#include "Candle/LightSource.hpp"

#include <algorithm>
#include <cmath>

#include "sfml-util/geometry/Line.hpp"
#include "sfml-util/geometry/Vector2.hpp"
#include "sfml-util/graphics/VertexArray.hpp"

namespace candle{
    std::vector<sfu::Line> LightSource::s_defaultSegmentPool;
    
    LightSource::LightSource()
        : m_glow(true)
        , m_color(sf::Color::White)
        , m_fade(true)
#ifdef CANDLE_DEBUG
        , m_debug(sf::Lines, 0)
#endif
        {
        m_ptrSegmentPool.insert(&s_defaultSegmentPool);
    }
    
    void LightSource::setIntensity(float intensity){
        m_color.a = 255 * intensity;
        sfu::setColor(m_polygon, m_color);
    }
    
    float LightSource::getIntensity() const{
        return (float)m_color.a/255.f;
    }
    
    void LightSource::setColor(const sf::Color& c){
        m_color = {c.r, c.g, c.b, m_color.a};
        sfu::setColor(m_polygon, m_color);
    }
    
    sf::Color LightSource::getColor() const{
        const sf::Color &c = m_color;
        return {c.r, c.g, c.b, 255};
    }
    
    void LightSource::setFade(bool fade){
        m_fade = fade;
    }
    
    bool LightSource::getFade() const{
        return m_fade;
    }
    
    void LightSource::setGlow(bool g){
        m_glow = g;
    }
    
    bool LightSource::getGlow() const{
        return m_glow;
    }
    
    float LightSource::getRange() const{
        return m_range;
    }
    
    bool LightSource::shouldRecast() const{
        return m_shouldRecast 
            || Transformable::getTransform() != m_transformOfLastCast;
    }
    
    sf::Vector2f LightSource::castRay(const sfu::Line r){
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
}
