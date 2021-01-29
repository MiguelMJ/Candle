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
     * @brief Interface for objects that emits light within a Lighting object
     * @details These objects are meant to be drawn as part of a Lighting object.
     */
    class LightSource: public sf::Transformable, private sf::Drawable{
    private:
        bool m_glow;
        
        /**
         * @brief This friendship is necessary to change the pointer of the 
         * segment pool
         * @see m_ptrSegmentPool Lighting::m_segmentPool
         */
        friend class Lighting;
        
        /**
         * @brief Draw the object to a target
         */
        virtual void draw(sf::RenderTarget& t, sf::RenderStates st) const = 0;
  
    protected:
        sf::Color m_color;
        sf::VertexArray m_polygon;
        float m_range;
        bool m_fade;
        bool m_shouldRecast;
        sf::Transform m_transformOfLastCast;
        
        /**
         * @brief Pools of segments that cast shadows.
         * @details By default, it points to @ref s_defaultSegmentPool.
         * @see s_defaultSegmentPool
         */
        std::set<std::vector<sfu::Line>*> m_ptrSegmentPool;

#ifdef CANDLE_DEBUG        
        sf::VertexArray m_debug;
#endif
        
    public:
        /**
         * @brief Default segment pool for shadow casting. Every LightSource contains it
         * in its own pool.
         * @see m_ptrSegmentPool
         */
        static std::vector<sfu::Line> s_defaultSegmentPool;
        
        /**
         * @brief Constructor
         */
        LightSource();
        
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
        void setColor(const sf::Color& color);
        
        /**
         * @brief Get the plain color of the light.
         * @details The alpha value is always 255.
         */
        sf::Color getColor() const;
        
        /**
         * @brief Set the falue of the _fade_ flag.
         * @details when the @p fade is set, the light will lose intensity
         * in the limits of its range. Otherwise, the intensity will remain
         * constant.
         * @param fade Value to set the flag.
         */
        virtual void setFade(bool fade);
        
        /**
         * @brief Check if the light fades or not.
         * @return The value of the _fade_ flag.
         */
        virtual bool getFade() const;
        
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
         * @brief Set the range of the illuminated area.
         * @param range Range of the illuminated area.
         */
        virtual void setRange(float range) = 0;
        
        /**
         * @brief Get the range of the illuminated area.
         */
        float getRange() const;
        
        /**
         * @brief Checks if the light may require a call to @ref castLight.
         * @details This function should be taken only as a guideline, as 
         * they are external factors that require a light to recast, so it
         * can return false negatives (but true will always be correct).
         */
        bool shouldRecast() const;
        
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
        virtual void castLight() = 0;
    };
}

#endif
