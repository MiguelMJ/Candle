#ifndef __CANDLE_DIRECTED_LIGHT_HPP__
#define __CANDLE_DIRECTED_LIGHT_HPP__

#include "Candle/LightSource.hpp"

namespace candle{
    class DirectedLight: public LightSource{
    private:
        float m_beamWidth;
        float m_beamInclination;
        
        void draw(sf::RenderTarget& t, sf::RenderStates st) const;
        
    public:
        DirectedLight();
        void setRange(float range);
        void castLight();
        void setBeamInclination(float angle);
        float getBeamInclination() const;
        void setBeamWidth(float width);
        float getBeamWidth() const;
        
    };
}

#endif
