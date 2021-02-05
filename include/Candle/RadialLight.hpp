#ifndef __CANDLE_RADIAL_LIGHT_HPP__
#define __CANDLE_RADIAL_LIGHT_HPP__

#include "Candle/LightSource.hpp"

namespace candle{
    class RadialLight: public LightSource{
    private:
        float m_beamAngle;
        
        void draw(sf::RenderTarget& t, sf::RenderStates st) const override;
        void resetColor() override;
        
    public:
        RadialLight();
        void castLight() override;
        void setBeamAngle(float angle);
        float getBeamAngle() const;
        
    };
}

#endif
