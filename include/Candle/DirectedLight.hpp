#ifndef __CANDLE_DIRECTED_LIGHT_HPP__
#define __CANDLE_DIRECTED_LIGHT_HPP__

#include "Candle/LightSource.hpp"

namespace candle{
    class DirectedLight: public LightSource{
    private:
        float m_beamWidth;
        
        void draw(sf::RenderTarget& t, sf::RenderStates st) const override;
        void resetColor() override;
    public:
        DirectedLight();
        void castLight() override;
        void setBeamWidth(float width);
        float getBeamWidth() const;
        
    };
}

#endif
