#include "Candle/LightSource.hpp"

#include <algorithm>
#include <cmath>

#include "sfml-util/geometry/Line.hpp"
#include "sfml-util/geometry/Vector2.hpp"

namespace candle{
    
    bool l_firstConstructor(true);
    sf::Texture l_lightTexture;
    
    void initializeLightTexture(){
        
    }
    
    std::vector<sfu::Line> LightSource::s_defaultSegmentPool;
    
    LightSource::LightSource()
        : m_polygon(sf::TriangleFan, 1)
        , m_color(sf::Color::White)
        , m_glow(true)
#ifdef CANDLE_DEBUG
        , m_debug(sf::Lines, 0)
#endif
        {
        // The first time we call a contructor, we create the texture
        // for all LightSources
        if(l_firstConstructor){
            float maxRadius = 100.0f;
            int points = 48;
            sf::RenderTexture lightTexture;
            lightTexture.create(maxRadius*2, maxRadius*2);
            sf::VertexArray lightShape(sf::TriangleFan, points+2);
            float step = M_PI*2/points;
            lightShape[0].position = {maxRadius,maxRadius};
            for(int i = 1; i < points+2; i++){
                lightShape[i].position = {
                    std::sin(step*(i))*maxRadius+maxRadius,
                    std::cos(step*(i))*maxRadius+maxRadius
                };
                lightShape[i].color.a = 0;
            }
            lightTexture.setSmooth(true);
            lightTexture.clear(sf::Color::Transparent);
            lightTexture.draw(lightShape);
            lightTexture.display();
            l_lightTexture = lightTexture.getTexture();
            
            l_firstConstructor = false;
        } // endif
        m_ptrSegmentPool.insert(&s_defaultSegmentPool);
        auto s = l_lightTexture.getSize();
        Transformable::setOrigin(s.x*0.5f, s.y*0.5f);
        m_bounds.width = s.x;
        m_bounds.height = s.y;
        setRadius(1.0f);
        setBeamAngle(360);
        castLight();
    }
    
    void LightSource::draw(sf::RenderTarget& t, sf::RenderStates s) const{
        s.transform *= Transformable::getTransform();
        s.texture = &l_lightTexture;
        t.draw(m_polygon, s);
#ifdef CANDLE_DEBUG
        sf::RenderStates debs;
        debs.transform = s.transform;
        auto gb = getLocalBounds();
        sf::RectangleShape debr(sf::Vector2f(gb.left,gb.top));
        debr.setFillColor(sf::Color::Transparent);
        debr.setOutlineThickness(1);
        debr.setSize(sf::Vector2f(gb.width, gb.height));
        t.draw(debr, debs);
        t.draw(m_debug, debs);
#endif
    }
    
    sf::FloatRect LightSource::getGlobalBounds() const{
        return Transformable::getTransform().transformRect(m_bounds);
    }
    sf::FloatRect LightSource::getLocalBounds() const{
        return m_bounds;
    }
    
    void LightSource::setIntensity(float intensity){
        m_color.a = 255 * intensity;
        m_polygon[0].color = m_color;
    }
    float LightSource::getIntensity() const{
        return (float)m_color.a/255;
    }
    void LightSource::setColor(sf::Color c){
        m_color.r = c.r;
        m_color.g = c.g;
        m_color.b = c.b;
        m_polygon[0].color = m_color;
    }
    sf::Color LightSource::getColor() const{
        sf::Color color = m_color;
        color.a = 255;
        return color;
    }
    void LightSource::setRadius(float r){
        r /= 100;
        r = std::max(0.f,r);
        Transformable::setScale(r,r);
    }
    float LightSource::getRadius() const{
        return Transformable::getScale().x;
    }
    void LightSource::setBeamAngle(float r){
        r = (float)std::fmod(r, 360);
        m_beamLimit1 = {std::cos(-r/2), std::sin(-r/2)};
        m_beamLimit2 = {std::cos(r/2), std::sin(r/2)};
    }
    float LightSource::getBeamAngle() const{
        return sfu::angle(m_beamLimit1, m_beamLimit2);
    }
    void LightSource::setGlow(bool g){
        m_glow = g;
    }
    bool LightSource::getGlow() const{
        return m_glow;
    }
    void LightSource::castLight(){
        //auto bounds = candle::segments(Transformable::getTransform().transformRect(m_bounds));
        auto castRay = [&] (const sfu::Line r) -> sf::Vector2f {
            sf::Vector2f ret(r.m_origin);   
            float minRange = std::numeric_limits<float>::infinity();
            for(auto& pool: m_ptrSegmentPool){
                for(auto& seg : *pool){
                    float t_seg, t_ray;
                    if(
                        seg.intersection(r, t_seg, t_ray) == sfu::Line::SECANT
                        && t_ray <= minRange
                        && t_ray >= 0.f
                        && t_seg <= 1.f
                        && t_seg >= 0.f
                    ){
                        minRange = t_ray;
                        ret = r.m_origin + t_ray*r.m_direction;
                    }
                }
            }
            return ret;
        };
        std::vector<sfu::Line> rays;
        int s = 0;
        for(auto& pool : m_ptrSegmentPool){
            s += pool->size();
        }
        rays.reserve(2 + 4 + s * 2 * 3); // 2: beam angle, 4: corners of bounds, 2: pnts/sgmnt, 3 rays/pnt
        
        // Start casting
        auto castPoint = Transformable::getPosition();
        float off = .001f;
        for(auto& pool : m_ptrSegmentPool){
            for(auto& s : *pool){
                sfu::Line r1(castPoint, s.m_origin);
                sfu::Line r2(castPoint, s.m_origin+s.m_direction);
                float a1 = sfu::angle(r1.m_direction);
                float a2 = sfu::angle(r2.m_direction);
                rays.push_back(r1);
                rays.emplace_back(castPoint, a1 - off);
                rays.emplace_back(castPoint, a1 + off);
                rays.push_back(r2);
                rays.emplace_back(castPoint, a2 - off);
                rays.emplace_back(castPoint, a2 + off);
                
            }
        }
        std::sort(
            rays.begin(),
            rays.end(),
            [] (sfu::Line& r1, sfu::Line& r2){
                return sfu::angle(r1.m_direction) < sfu::angle(r2.m_direction);
            }
        );
        sf::Transform tr_i = Transformable::getTransform().getInverse();
        // keep only the ones within the area
        std::vector<sf::Vector2f> points;
        points.reserve(rays.size());
        if(360 - sfu::angle(m_beamLimit1, m_beamLimit2) < 0.1){
            for (auto& r: rays){
                points.push_back(tr_i.transformPoint(castRay(r)));
            }
        }else{
            sf::Vector2f o = getPosition();
            for (auto& r: rays){
                sf::Vector2f p = castRay(r);
                sfu::Line beamLimit1(o, o+m_beamLimit1);
                sfu::Line beamLimit2(o, o+m_beamLimit2);
                if(beamLimit2.relativePosition(p) - beamLimit1.relativePosition(p) >= 0){
                    points.push_back(tr_i.transformPoint(p));
                }
            }
        }
#ifdef CANDLE_DEBUG
        m_debug.resize(rays.size()*2);
#endif
        m_polygon.resize(rays.size() + 2); // + center and last
        m_polygon[0].color = m_color;
        m_polygon[0].position = 
        m_polygon[0].texCoords = tr_i.transformPoint(castPoint);
        for(unsigned i=0; i < rays.size(); i++){
            sf::Vector2f p = tr_i.transformPoint(castRay(rays[i]));
            m_polygon[i+1].position = p;
            m_polygon[i+1].texCoords = p;
            m_polygon[i+1].color = m_color;
#ifdef CANDLE_DEBUG
            m_debug[i*2].position = m_polygon[0].position;
            m_debug[i*2+1].position = p;
            m_debug[i*2].color = m_debug[i*2+1].color = sf::Color::Magenta;
#endif            
        }
        m_polygon[rays.size()+1] = m_polygon[1];
    }
}
