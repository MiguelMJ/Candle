#ifdef CANDLE_DEBUG
#include <iostream>
#endif  

#include <memory>
#include "Candle/PointLight.hpp"

#include "SFML/Graphics.hpp"

#include "Candle/graphics/VertexArray.hpp"
#include "Candle/geometry/Vector2.hpp"
#include "Candle/geometry/Line.hpp"

namespace candle{

    namespace {
        const char *shaderText = R"===(
            uniform vec2 center;
            uniform float radius;
            uniform vec4 color;
            uniform float bleed;
            uniform float intensity;
            uniform float linearFactor;
            
            void main() {
                vec2 pixel = gl_FragCoord.xy;

                float dist = length(center - pixel);

                float distFromFalloff = radius - dist;

                float attenuation = 0.0;
                attenuation = distFromFalloff * (bleed / (dist*dist) + linearFactor/radius);
                        
                attenuation = clamp(attenuation, 0.0, 1.0);

                gl_FragColor = vec4(color.rgb, attenuation * intensity);
            }
        )===";
        sf::Shader g_pointLightShader;
    }

    float module360(float x){
        x = (float)fmod(x,360.f);
        if(x < 0.f) x += 360.f;
        return x;
    }

    PointLight::PointLight() : LightSource() {
        // check if the shader is loaded and do it if necessary
        if(g_pointLightShader.getNativeHandle() == 0){
            g_pointLightShader.loadFromMemory(shaderText, sf::Shader::Fragment);
        }

        m_polygon.setPrimitiveType(sf::TriangleFan);
        m_polygon.resize(6);
        m_polygon[0].position ={0.f, 0.f};
        m_polygon[1].position = {-1.f, -1.f};
        m_polygon[2].position = {1.f, -1.f};
        m_polygon[3].position = {1.f, 1.f};
        m_polygon[4].position = {-1.f, 1.f};
        m_polygon[5].position = {-1.f, -1.f};
        
        setRange(1.0f);
        setBeamAngle(360.f);
        setLinearFactor(1.f);
        setIntensity(1.f);
        setBleed(0.f);
    }



    void PointLight::draw(sf::RenderTarget& target, sf::RenderStates renderStates) const{
        
        sf::Vector2f shaderCenter(getPosition().x, target.getSize().y-getPosition().y);

        g_pointLightShader.setUniform("center", shaderCenter);
        g_pointLightShader.setUniform("radius", m_range);
        g_pointLightShader.setUniform("color", sf::Glsl::Vec4(m_color));
        g_pointLightShader.setUniform("bleed", m_bleed);
        g_pointLightShader.setUniform("intensity", m_intensity);
        g_pointLightShader.setUniform("linearFactor", m_linearFactor);
        
        sf::Transform transform = Transformable::getTransform();
        transform.scale(m_range, m_range);
        renderStates.transform *= transform;

        renderStates.shader = &g_pointLightShader;
        if(renderStates.blendMode == sf::BlendAlpha){
            renderStates.blendMode = sf::BlendAdd;
        }
        
        target.draw(m_polygon, renderStates);

#ifdef CANDLE_DEBUG
        sf::RenderStates deb_s;
        deb_s.transform = renderStates.transform;
        target.draw(m_debug, deb_s);
#endif

    }

    void PointLight::setBeamAngle(float r){
        m_beamAngle = module360(r);
    }

    float PointLight::getBeamAngle() const{
        return m_beamAngle;
    }

    sf::FloatRect PointLight::getLocalBounds() const{
        return sf::FloatRect(-1.0f, -1.0f, 2.0f, 2.0f);
    }

    sf::FloatRect PointLight::getGlobalBounds() const{
        sf::Transform transform = Transformable::getTransform();
        transform.scale(m_range, m_range);
        return transform.transformRect( getLocalBounds() );
    }

    void PointLight::castLight(const EdgeVector::iterator& beginEdges, const EdgeVector::iterator& endEdges){
        sf::Transform transform = Transformable::getTransform();
        transform.scale(m_range, m_range);
        std::vector<sfu::Line> rays;

        rays.reserve(2 + std::distance(beginEdges, endEdges) * 2 * 3); // 2: beam angle, 4: corners, 2: pnts/sgmnt, 3 rays/pnt

        // Start casting
        float beamLowerLimit = module360(getRotation() - m_beamAngle/2);
        float beamUpperLimit = module360(getRotation() + m_beamAngle/2);
        bool isBeamTotallyOpen = m_beamAngle < 0.1f; // 360 will turn 0. See setBeamAngle
        auto castOriginPoint = Transformable::getPosition();
        float angleOffsetForEdgeEnd = .001f;

        auto isAngleInsideBeam = [&](float a)-> bool {
            return isBeamTotallyOpen
                   ||(beamLowerLimit < beamUpperLimit && a > beamLowerLimit && a < beamUpperLimit)
                   ||(beamLowerLimit > beamUpperLimit && (a > beamLowerLimit || a < beamUpperLimit));
        };

        for(float a = 45.f; a < 360.f; a += 90.f){
            if(isAngleInsideBeam(a)){
                rays.emplace_back(castOriginPoint, a);
            }
        }

        sf::FloatRect lightGlobalBounds = getGlobalBounds();
        for(auto it = beginEdges; it != endEdges; it++){
            auto& edge = *it;

            //Only cast a ray if the line is in range
            if( lightGlobalBounds.intersects( edge.getGlobalBounds() ) ){
                sfu::Line ray1(castOriginPoint, edge.m_origin);
                sfu::Line ray2(castOriginPoint, edge.point(1.f));
                float angleRay1 = sfu::angle(ray1.m_direction);
                float angleRay2 = sfu::angle(ray2.m_direction);
                if(isAngleInsideBeam(angleRay1)){
                    rays.push_back(ray1);
                    rays.emplace_back(castOriginPoint, angleRay1 - angleOffsetForEdgeEnd);
                    rays.emplace_back(castOriginPoint, angleRay1 + angleOffsetForEdgeEnd);
                }
                if(isAngleInsideBeam(angleRay2)){
                    rays.push_back(ray2);
                    rays.emplace_back(castOriginPoint, angleRay2 - angleOffsetForEdgeEnd);
                    rays.emplace_back(castOriginPoint, angleRay2 + angleOffsetForEdgeEnd);
                }
            }
        }

        if(beamLowerLimit > beamUpperLimit){
            std::sort(
                rays.begin(),
                rays.end(),
                [beamLowerLimit, beamUpperLimit] (sfu::Line& ray1, sfu::Line& ray2){
                    float realBeamLowerLimit = beamLowerLimit-0.1;
                    float realBeamUpperLimit = beamUpperLimit+0.1;
                    float a1 = sfu::angle(ray1.m_direction);
                    float a2 = sfu::angle(ray2.m_direction);
                    return (a1 >= realBeamLowerLimit && a2 <= realBeamUpperLimit) || (a1 < a2 && (realBeamLowerLimit <= a1 || a2 <= realBeamUpperLimit));
                }
            );
        }else{
            std::sort(
                rays.begin(),
                rays.end(),
                [beamLowerLimit] (sfu::Line& ray1, sfu::Line& ray2){
                    return
                        sfu::angle(ray1.m_direction) < sfu::angle(ray2.m_direction);
                }
            );
        }

        if(!isBeamTotallyOpen){
            rays.emplace(rays.begin(), castOriginPoint, beamLowerLimit);
            rays.emplace_back(castOriginPoint, beamUpperLimit);
        }

        sf::Transform inverseTransform = transform.getInverse();
        // keep only the ones within the area
        std::vector<sf::Vector2f> raycastHits;
        raycastHits.reserve(rays.size());
        for (auto& ray: rays){
            raycastHits.push_back(inverseTransform.transformPoint(castRay(beginEdges, endEdges, ray, m_range*m_range)));
        }
        m_polygon.resize(raycastHits.size() + 1 + isBeamTotallyOpen); // + center and last
        m_polygon[0].color = m_color;
        m_polygon[0].position = m_polygon[0].texCoords = inverseTransform.transformPoint(castOriginPoint);
#ifdef CANDLE_DEBUG
        float bl1rad = beamLowerLimit * sfu::PI/180.f;
        float bl2rad = beamUpperLimit * sfu::PI/180.f;
        sf::Vector2f al1(std::cos(bl1rad), std::sin(bl1rad));
        sf::Vector2f al2(std::cos(bl2rad), std::sin(bl2rad));
        int d_n = raycastHits.size()*2 + 4;
        m_debug.resize(d_n);
        m_debug[d_n-1].color = m_debug[d_n-2].color = sf::Color::Cyan;
        m_debug[d_n-3].color = m_debug[d_n-4].color = sf::Color::Yellow;
        m_debug[d_n-1].position = m_debug[d_n-3].position = m_polygon[0].position;
        m_debug[d_n-2].position = inverseTransform.transformPoint(castOriginPoint + m_range * al1);
        m_debug[d_n-4].position = inverseTransform.transformPoint(castOriginPoint + m_range * al2);
#endif
        for(unsigned i=0; i < raycastHits.size(); i++){
            sf::Vector2f raycastHit = raycastHits[i];
            m_polygon[i+1].position = raycastHit;
            m_polygon[i+1].texCoords = raycastHit;
            m_polygon[i+1].color = m_color;
#ifdef CANDLE_DEBUG
            m_debug[i*2].position = m_polygon[0].position;
            m_debug[i*2+1].position = raycastHit;
            m_debug[i*2].color = m_debug[i*2+1].color = sf::Color::Magenta;
#endif
        }
        if(isBeamTotallyOpen){
            m_polygon[raycastHits.size()+1] = m_polygon[1];
        }
    }

}
