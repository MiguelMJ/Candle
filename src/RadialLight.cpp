#include "Candle/RadialLight.hpp"

#include "SFML/Graphics.hpp"

#include "sfml-util/graphics/VertexArray.hpp"
#include "sfml-util/geometry/Vector2.hpp"
#include "sfml-util/geometry/Line.hpp"

namespace candle{
    const float BASE_RADIUS = 100.0f;
    bool l_firstConstructor(true);
    sf::Texture l_lightTextureFade;
    sf::Texture l_lightTexturePlain;
      
    void initializeTextures(){
        int points = 48;
        
        sf::RenderTexture lightTextureFade, lightTexturePlain;
        lightTextureFade.create(BASE_RADIUS*2, BASE_RADIUS*2);
        lightTexturePlain.create(BASE_RADIUS*2, BASE_RADIUS*2);
        
        sf::VertexArray lightShape(sf::TriangleFan, points+2);
        float step = M_PI*2/points;
        lightShape[0].position = {BASE_RADIUS,BASE_RADIUS};
        for(int i = 1; i < points+2; i++){
            lightShape[i].position = {
                std::sin(step*(i))*BASE_RADIUS+BASE_RADIUS,
                std::cos(step*(i))*BASE_RADIUS+BASE_RADIUS
            };
            lightShape[i].color.a = 0;
        }
        lightTextureFade.setSmooth(true);
        lightTextureFade.clear(sf::Color::Transparent);
        lightTextureFade.draw(lightShape);
        lightTextureFade.display();
        
        sfu::setColor(lightShape, sf::Color::White);
        lightTexturePlain.setSmooth(true);
        lightTexturePlain.clear(sf::Color::Transparent);
        lightTexturePlain.draw(lightShape);
        lightTexturePlain.display();
        
        l_lightTextureFade = lightTextureFade.getTexture();
        l_lightTexturePlain = lightTexturePlain.getTexture();
    }
    
    float module360(float x){
        x = (float)fmod(x,360.f);
        if(x < 0.f) x += 360.f;
        return x;
    }
    
    RadialLight::RadialLight()
        : LightSource()
        {
        if(l_firstConstructor){
            // The first time we create a RadialLight, we must create the textures
            initializeTextures();
            l_firstConstructor = false;
        }
        m_polygon.setPrimitiveType(sf::TriangleFan);
        Transformable::setOrigin(BASE_RADIUS, BASE_RADIUS);
        setRange(1.0f);
        setBeamAngle(360.f);
        m_shouldRecast = true;
        // castLight();
    }
    
    void RadialLight::draw(sf::RenderTarget& t, sf::RenderStates s) const{
        sf::Transform trm = Transformable::getTransform();
        trm.scale(m_range, m_range, BASE_RADIUS, BASE_RADIUS);
        s.transform *= trm;
        s.texture = m_fade ? &l_lightTextureFade : &l_lightTexturePlain;
        t.draw(m_polygon, s);
#ifdef CANDLE_DEBUG
        sf::RenderStates deb_s;
        deb_s.transform = s.transform;
        t.draw(m_debug, deb_s);
#endif
    }
    
    void RadialLight::setRange(float r){
        r /= BASE_RADIUS;
        m_range = std::max(r, 0.f);
        m_shouldRecast = true;
    }
    
    void RadialLight::setBeamAngle(float r){
        m_beamAngle = module360(r);
        m_shouldRecast = true;
    }
    
    float RadialLight::getBeamAngle() const{
        return m_beamAngle;
    }
    
    void RadialLight::castLight(){
        sf::Transform trm = Transformable::getTransform();
        trm.scale(m_range, m_range, BASE_RADIUS, BASE_RADIUS);
        std::vector<sfu::Line> rays;
        int s = 0;
        for(auto& pool : m_ptrSegmentPool){
            s += pool->size();
        }
        rays.reserve(2 + s * 2 * 3); // 2: beam angle, 2: pnts/sgmnt, 3 rays/pnt
        
        // Start casting
        float bl1 = module360(getRotation() - m_beamAngle/2);
        float bl2 = module360(getRotation() + m_beamAngle/2);
        bool beamAngleBigEnough = m_beamAngle < 0.1;
        auto castPoint = Transformable::getPosition();
        float off = .001f;
        for(auto& pool : m_ptrSegmentPool){
            for(auto& s : *pool){
                sfu::Line r1(castPoint, s.m_origin);
                sfu::Line r2(castPoint, s.m_origin+s.m_direction);
                float a1 = sfu::angle(r1.m_direction);
                float a2 = sfu::angle(r2.m_direction);
                if(
                    beamAngleBigEnough 
                    || (
                        bl1 < bl2
                        && a1 > bl1
                        && a1 < bl2
                    )
                    || (
                        bl1 > bl2
                        && (
                            a1 > bl1
                            || a1 < bl2
                        )
                    )
                ){
                    rays.push_back(r1);
                    rays.emplace_back(castPoint, a1 - off);
                    rays.emplace_back(castPoint, a1 + off);
                }
                if(
                    beamAngleBigEnough 
                    || (
                        bl1 < bl2
                        && a2 > bl1
                        && a2 < bl2
                    )
                    || (
                        bl1 > bl2
                        && (
                            a2 > bl1
                            || a2 < bl2
                        )
                    )
                ){
                    rays.push_back(r2);
                    rays.emplace_back(castPoint, a2 - off);
                    rays.emplace_back(castPoint, a2 + off);
                }
                
            }
        }
        if(bl1 > bl2){
            std::sort(
                rays.begin(),
                rays.end(),
                [bl1, bl2] (sfu::Line& r1, sfu::Line& r2){
                    float _bl1 = bl1-0.1;
                    float _bl2 = bl2+0.1;
                    float a1 = sfu::angle(r1.m_direction);
                    float a2 = sfu::angle(r2.m_direction);
                    return (a1 >= _bl1 && a2 <= _bl2) || (a1 < a2 && (_bl1 <= a1 || a2 <= _bl2));
                }
            );
        }else{
            std::sort(
                rays.begin(),
                rays.end(),
                [bl1] (sfu::Line& r1, sfu::Line& r2){
                    return 
                        sfu::angle(r1.m_direction) < sfu::angle(r2.m_direction);
                }
            );
        }
        if(!beamAngleBigEnough){
            rays.emplace(rays.begin(), castPoint, bl1);
            rays.emplace_back(castPoint, bl2);
        }
        
        sf::Transform tr_i = trm.getInverse();
        // keep only the ones within the area
        std::vector<sf::Vector2f> points;
        points.reserve(rays.size());
        for (auto& r: rays){
            points.push_back(tr_i.transformPoint(castRay(r)));
        }
        m_polygon.resize(points.size() + 1 + beamAngleBigEnough); // + center and last
        m_polygon[0].color = m_color;
        m_polygon[0].position = m_polygon[0].texCoords = tr_i.transformPoint(castPoint);
#ifdef CANDLE_DEBUG
        float bl1rad = bl1 * M_PI/180.f;
        float bl2rad = bl2 * M_PI/180.f;
        sf::Vector2f al1(std::cos(bl1rad), std::sin(bl1rad));
        sf::Vector2f al2(std::cos(bl2rad), std::sin(bl2rad));
        int d_n = points.size()*2 + 4;
        m_debug.resize(d_n);
        m_debug[d_n-1].color = m_debug[d_n-2].color = sf::Color::Cyan; 
        m_debug[d_n-3].color = m_debug[d_n-4].color = sf::Color::Yellow;
        m_debug[d_n-1].position = m_debug[d_n-3].position = m_polygon[0].position;
        m_debug[d_n-2].position = tr_i.transformPoint(castPoint + m_range * al1);
        m_debug[d_n-4].position = tr_i.transformPoint(castPoint + m_range * al2);
#endif
        for(unsigned i=0; i < points.size(); i++){
            sf::Vector2f p = points[i];
            m_polygon[i+1].position = p;
            m_polygon[i+1].texCoords = p;
            m_polygon[i+1].color = m_color;
#ifdef CANDLE_DEBUG
            m_debug[i*2].position = m_polygon[0].position;
            m_debug[i*2+1].position = p;
            m_debug[i*2].color = m_debug[i*2+1].color = sf::Color::Magenta;
#endif            
        }
        if(beamAngleBigEnough){
            m_polygon[points.size()+1] = m_polygon[1];
        }
        m_transformOfLastCast = Transformable::getTransform();
        m_shouldRecast = false;
    }
}
