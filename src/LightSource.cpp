#include "Candle/LightSource.hpp"

#include <algorithm>
#include <cmath>

#include "Candle/geometry/Line.hpp"
#include "Candle/geometry/Vector2.hpp"
#include "Candle/graphics/VertexArray.hpp"

namespace candle{
    LightSource::LightSource()
        : m_color(sf::Color::White)
        , m_fade(true)
#ifdef CANDLE_DEBUG
        , m_debug(sf::Lines, 0)
#endif
        {}
    
    void LightSource::setIntensity(float intensity){
        m_color.a = 255 * intensity;
        resetColor();
    }
    
    float LightSource::getIntensity() const{
        return (float)m_color.a/255.f;
    }
    
    void LightSource::setColor(const sf::Color& c){
        m_color = {c.r, c.g, c.b, m_color.a};
        resetColor();
    }
    
    sf::Color LightSource::getColor() const{
        const sf::Color &c = m_color;
        return {c.r, c.g, c.b, 255};
    }
    
    void LightSource::setFade(bool fade){
        m_fade = fade;
        resetColor();
    }
    
    bool LightSource::getFade() const{
        return m_fade;
    }
    
    void LightSource::setRange(float r){
        m_range = r;
    }
    
    float LightSource::getRange() const{
        return m_range;
    }
    
}
