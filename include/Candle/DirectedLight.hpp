/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the RadialLight class.
 */
#ifndef __CANDLE_DIRECTED_LIGHT_HPP__
#define __CANDLE_DIRECTED_LIGHT_HPP__

#include "Candle/LightSource.hpp"

namespace candle{
    /**
     * @brief LightSource that emits light in a single direction.
     * @details 
     * 
     * A DirectedLight is defined, mainly, by the direction of the rays, the 
     * position of the source, the beam width and the range of the light. You
     * can manipulate the two first transforming changing the rotation and 
     * position as you would do with any sf::Transformable. The range can
     * be manipulated as with other LightSources, with
     * @ref LightSource::setRange, and the beam width with @ref setBeamWidth.
     * 
     * <table width="100%">
     * <tr>
     *  <td align="center"> <img src="directed_1.png" width="300px"> <br> <em>Variables schema</em> </td>
     *  <td align="center"> <img src="directed_2.png" width="300px"> <br> <em>Demo example</em> </td>
     * </tr>
     * </table>
     * 
     */
    class DirectedLight: public LightSource{
    private:
        float m_beamWidth;
        
        void draw(sf::RenderTarget& t, sf::RenderStates st) const override;
        void resetColor() override;
    public:
        DirectedLight();
        
        void castLight(const EdgeVector::iterator& begin, const EdgeVector::iterator& end) override;
        
        /**
         * @brief Set the width of the beam.
         * @details The width specifies the maximum distance allowed from the 
         * center of segment to cast a ray, along a segment normal to the ligh
         * direction.
         * @param width Width of the beam.
         * @see getBeamWidth
         */
        void setBeamWidth(float width);
        
        /**
         * @brief Get the width of the beam.
         * @returns Width of the beam
         * @see setBeamWidth
         */
        float getBeamWidth() const;
        
    };
}

#endif
