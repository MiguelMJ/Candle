/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the RadialLight class.
 */
#ifndef __CANDLE_RADIAL_LIGHT_HPP__
#define __CANDLE_RADIAL_LIGHT_HPP__

#include "Candle/LightSource.hpp"

namespace candle{
    /**
     * @brief LightSource that emits light from a single point
     * @details
     *
     * A RadialLight is defined, mainly, by the position, the orientation, the
	 * range of the light and the beam angle. To manipulate the
     * position and the orientation of the light, you can change the position
	 * and rotation of the object as you would do with any sf::Transformable.
	 * The range can be manipulated as with other LightSources, with
     * @ref LightSource::setRange, and the angle of the beam with
	 * @ref setBeamAngle.
     *
     * <table width="100%">
     * <tr>
     *  <td align="center"> <img src="radial_1.png" width="300px"> <br> <em>Variables schema</em> </td>
     *  <td align="center"> <img src="radial_2.png" width="300px"> <br> <em>Demo example</em> </td>
     * </tr>
     * </table>
     */
    class RadialLight: public LightSource{
    private:
        static int s_instanceCount;
        float m_beamAngle;
        unsigned int m_localRadius;

        sf::Texture* m_lightTextureFade;
        sf::Texture* m_lightTexturePlain;

        void draw(sf::RenderTarget& t, sf::RenderStates st) const override;
        void resetColor() override;

    public:
        /**
         * @brief Constructor
         */
        RadialLight();

        /**
         * @brief Destructor
         */
        virtual ~RadialLight();

        void castLight(const EdgeVector::iterator& begin, const EdgeVector::iterator& end) override;

        void setTexture(sf::Texture* texture) override;

        /**
         * @brief Set the range for which rays may be casted.
         * @details The angle shall be specified in degrees. The angle in which the rays will be casted will be
         * [R - angle/2, R + angle/2], where R is the rotation of the object.
         * @param angle
         * @see getBeamAngle
         */
        void setBeamAngle(float angle);

        /**
         * @brief Get the range for which rays may be casted.
         * @details It defaults to 360º.
         * @see setBeamAngle
         */
        float getBeamAngle() const;

        /**
         * @brief Get the light fade texture.
         * @returns The actual light fade texture.
         * @see setLightFadeTexture
         */
        sf::Texture* getLightFadeTexture();

        /**
         * @brief Get the light plain texture.
         * @returns The actual light plain texture.
         * @see setLightPlainTexture
         */
        sf::Texture* getLightPlainTexture();

        /**
         * @brief Get the local bounding rectangle of the light.
         * @returns The local bounding rectangle in float.
         */
        sf::FloatRect getLocalBounds() const;

        /**
         * @brief Get the global bounding rectangle of the light.
         * @returns The global bounding rectangle in float.
         */
        sf::FloatRect getGlobalBounds() const;

    };
}

#endif
