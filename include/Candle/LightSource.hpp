/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the LightingSource class.
 */
#ifndef __CANDLE_LIGHTSOURCE_HPP__
#define __CANDLE_LIGHTSOURCE_HPP__

#include <vector>
#include <set>

#include "SFML/Graphics.hpp"

#include "sfml-util/geometry/Line.hpp"

namespace candle{
    /**
     * @brief Object that emits light within a 
     * Lighting object
     * @details These objects are meant to be drawn as part of a Lighting object.
     */
    class LightSource: public sf::Transformable, private sf::Drawable{
    private:
        sf::VertexArray m_polygon;
        sf::FloatRect m_bounds; 
        sf::Color m_color;
        bool m_glow;
        float m_beamAngle;
        sf::Vector2f m_beamLimit1;
        sf::Vector2f m_beamLimit2;
#ifdef CANDLE_DEBUG
        sf::VertexArray m_debug;
#endif
        
        /**
         * @brief Pools of segments that cast shadows.
         * @details By default, it points to @ref s_defaultSegmentPool.
         * @see s_defaultSegmentPool
         */
        std::set<std::vector<sfu::Line>*> m_ptrSegmentPool;
        
        /**
         * @brief This friendship is necessary to change the pointer of the 
         * segment pool
         * @see m_ptrSegmentPool Lighting::m_segmentPool
         */
        friend class Lighting;
        
        /**
         * @brief Draw the object to a target
         */
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    public:
        
        /**
         * @brief Default segment pool for shadow casting. Every LightSource contains it
         * in its own pool.
         * @see m_ptrSegmentPool
         */
        static std::vector<sfu::Line> s_defaultSegmentPool;
        
        /**
         * @brief Constructor.
         */
        LightSource();
                
        /**
         * @brief Get the global bounding rectangle of the light source.
         */
        sf::FloatRect getGlobalBounds() const;
        
        /**
         * @brief Get the local bounding rectangle of the light source.
         */
        sf::FloatRect getLocalBounds() const;
        
        /**
         * @brief Set the light intensity.
         * @details The @p intensity of the light determines two things: 
         * how much fog opacity it reduces, and how much of its color is 
         * added to the layers below when the _glow_ is active.
         * @param intensity Value from 0 to 1. At 0 the light is
         * invisible.
         * @see setGlow
         */
        void setIntensity(float intensity);
        
        /**
         * @brief Get the intensity of the light.
         */
        float getIntensity() const;
        
        /**
         * @brief Set the light color.
         * @details The light will only show color if the _glow_ is 
         * active.
         * @param color New color of the light.
         * @see setGlow
         */
        void setColor(sf::Color color);
        
        /**
         * @brief Get the plain color of the light.
         * @details The alpha value is always 255.
         */
        sf::Color getColor() const;
        
        /**
         * @brief Set the radius of the iluminated area.
         * @param radius New radius of the area.
         */
        void setRadius(float radius);
        
        /**
         * @brief Get the radius of the light.
         */
        float getRadius() const;
        
        /**
         * @brief Set the beam angle of the light.
         * @param angle New beam angle.
         */
        void setBeamAngle(float angle);
        
        /**
         * @brief Get the beam angle of the light.
         */
        float getBeamAngle() const;
        
        /**
         * @brief Set the value of the _glow_ flag.
         * @details When the @p glow is inactive, the @ref Lighting 
         * will only use the light to reveal the area under the fog. If 
         * it is active, it will also add the color to the image. Note 
         * that when the glow is not set, the light won't be visible if 
         * the [fog opacity](@ref Lighting::setFogOpacity) is 0.
         * @param glow Value to set the flag.
         */
        void setGlow(bool glow);
        
        /**
         * @brief Check if the light glows or not.
         * @return The value of the _glow_ flag.
         */
        bool getGlow() const;
        
        /**
         * @brief Calculates the area that should be iluminated with a 
         * ray casting algorithm.
         * @details For the calculations, the segments from @ref 
         * m_ptrSegmentPool are used. If the [fog opacity](@ref 
         * Lighting::setFogOpacity) is not 0, then @ref 
         * Lighting::updateFog should be called somewhere between 
         * this function and the next draw.
         * @see m_ptrSegmentPool
         */
        void castLight();
    };
}

#endif
