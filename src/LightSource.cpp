#include "Candle/LightSource.hpp"

#include <algorithm>

#include "Candle/Constants.hpp"
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
        m_intensity = intensity;
    }
    
    float LightSource::getIntensity() const{
        return m_intensity;
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
    
    void LightSource::setLinearFactor(float lf){
        m_linearFactor = lf;
    }
    
    float LightSource::getLinearFactor() const{
        return m_linearFactor;
    }
    
    void LightSource::setBleed(float b){
        m_bleed = b;
    }
    
    float LightSource::getBleed() const{
        return m_bleed;
    }
    
}
