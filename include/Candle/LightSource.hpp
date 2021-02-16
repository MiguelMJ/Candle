/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the LightingSource class and some convenient
 * typedefs.
 */
#ifndef __CANDLE_LIGHTSOURCE_HPP__
#define __CANDLE_LIGHTSOURCE_HPP__

#include <vector>

#include "SFML/Graphics.hpp"

#include "Candle/geometry/Line.hpp"

namespace candle{
    /**
     * @typedef Edge
     * @brief Typedef with mere semantic purposes.
     */
    typedef sfu::Line Edge;
    
    /**
     * @typedef EdgeVector
     * @brief Typedef to shorten the use of vectors as edge pools
     */
    typedef std::vector<Edge> EdgeVector;
    
    /**
     * @brief This function initializes the Texture used for the RadialLights.
     * @details This function is called the first time a RadialLight is created
     * , so the user shouldn't need to do it. Anyways, it could be 
     * necessary to do it explicitly if you declare a RadialLight that, for 
     * some reason, is global or static RadialLight and is not constructed in
     * a normal order.
     */
    void initializeTextures();
    
    /**
     * @brief Interface for objects that emit light
     * @details
     * 
     * LightSources use raycasting algorithms to compute the polygon
     * illuminated by the light. The main difference between the 
     * implementations, @ref RadialLight and @ref DirectedLight, is whether
     * the constant is the origin or the direction of the rays.
     * 
     * LightSources manage their colour separating the alpha value from the RGB
     * . This is convenient to manipulate color of the light (interpreted as 
     * the RGB value) and intensity (interpreted as the alpha value) 
     * separately.
     * 
     * By default, they use a sf::BlendAdd mode. This means that you can
     * specify any other blend mode you want except sf::BlendAlpha, that
     * will be converted to the additive mode.
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
         * how much fog opacity it reduces when drawn in a LightingArea * in 
         * FOG mode, and how much presence its color has when drawn normally.
         * 
         * The default value is 1.
         * 
         * @param intensity Value from 0 to 1. At 0 the light is
         * invisible.
         * @see getIntensity
         */
        void setIntensity(float intensity);
        
        /**
         * @brief Get the intensity of the light.
         * @returns The light intensity.
         * @see setIntensity
         */
        float getIntensity() const;
        
        /**
         * @brief Set the light color.
         * @details The light color refers only to the RGB values.
         * 
         * The default value is sf::Color::White
         * 
         * @param color New color of the light. The alpha value is ignored.
         * @see getColor
         */
        void setColor(const sf::Color& color);
        
        /**
         * @brief Get the plain color of the light.
         * @details The light color refers only to the RGB values.
         * @returns The light color.
         * @see setColor
         */
        sf::Color getColor() const;
        
        /**
         * @brief Set the value of the _fade_ flag.
         * @details when the @p fade flag is set, the light will lose intensity
         * in the limits of its range. Otherwise, the intensity will remain
         * constant.
         * 
         * The default value is true.
         * 
         * @param fade Value to set the flag.
         * @see getFade
         */
        virtual void setFade(bool fade);
        
        /**
         * @brief Check if the light fades or not.
         * @returns The value of the _fade_ flag.
         * @see setFade
         */
        virtual bool getFade() const;
            
        /**
         * @brief Set the range of the illuminated area.
         * @details The range of the light indicates the how far a light ray
         * may hit from its origin.
         * @param range Range of the illuminated area.
         * @see getRange, setFade
         */
        void setRange(float range);
        
        /**
         * @brief Get the range of the illuminated area.
         * @returns The range of the illuminated area.
         * @see setRange
         */
        float getRange() const;
        
        /**
         * @brief Modifies the polygon of the illuminated area with a 
         * raycasting algorithm.
         * @details The algorithm needs to know what edges to use to cast 
         * shadows. They are specified within a range of two iterators of a
         * vector of edges of type @ref sfu::Line.
         * @param begin Iterator to the first sfu::Line of the vector to take 
         * into account.
         * @param end Iterator to the first sfu::Line of the vector not to be
         * taken into account.
         * @see setRange, [EdgeVector](@ref LightSource.hpp)
         */
        virtual void castLight(const EdgeVector::iterator& begin, const EdgeVector::iterator& end) = 0;
    };
}

#endif
