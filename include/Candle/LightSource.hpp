/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the LightingSource class.
 */
#ifndef __CANDLE_LIGHTSOURCE_HPP__
#define __CANDLE_LIGHTSOURCE_HPP__

#include <vector>

#include "SFML/Graphics.hpp"

#include "sfml-util/geometry/Line.hpp"

namespace candle{
    typedef std::vector<sfu::Line> EdgeVector;
    /**
     * @brief This function initializes the Texture used for the RadialLights.
     * @details It is called byt the first constructor. Anyways, it could be 
     * necessary to call it explicitly if you declare a RadialLight that, for 
     * some reason, you have a global or static RadialLight.
     */
    void initializeTextures();
    
    /**
     * @brief Interface for objects that emit light
     * @details LightSources must be created and drawn from a @ref Lighting object.
     */
    class LightSource: public sf::Transformable, public sf::Drawable{
    private:
        /**
         * @brief Draw the object to a target
         */
        virtual void draw(sf::RenderTarget& t, sf::RenderStates st) const = 0;
  
    protected:
        sf::Color m_color;
        sf::VertexArray m_polygon;
        float m_range;
        float m_intensity; // only for fog
        bool m_fade;

#ifdef CANDLE_DEBUG        
        sf::VertexArray m_debug;
#endif
        
        virtual void resetColor() = 0;
    
    public:
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
         * @brief Set the range of the illuminated area.
         * @param range Range of the illuminated area.
         */
        void setRange(float range);
        
        /**
         * @brief Get the range of the illuminated area.
         */
        float getRange() const;
        
        /**
         * @brief Calculates the area that should be iluminated with a 
         * ray casting algorithm.
         * @details For the calculations, the edges from @ref 
         * m_ptrEdgePool are used. If the [fog opacity](@ref 
         * Lighting::setFogOpacity) is not 0, then @ref 
         * Lighting::updateFog should be called somewhere between 
         * this function and the next draw.
         * @see m_ptrEdgePool
         */
        virtual void castLight(const EdgeVector::iterator& begin, const EdgeVector::iterator& end) = 0;
    };
}

#endif
