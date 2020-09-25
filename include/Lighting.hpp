/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the Lighting class.
 */
#ifndef __CANDLE_LIGHTING_HPP__
#define __CANDLE_LIGHTING_HPP__

#include <set>

#include "SFML/Graphics.hpp"

#include "Util.hpp"
#include "LightSource.hpp"

namespace candle{
    /**
     * @brief Object to manage light, fog and shadows in an area.
     * @details ...
     */
    class Lighting: public sf::Drawable{
    private:
        std::set <const LightSource*> m_lights;
        sf::RenderTexture m_fogTexture;
        sf::VertexArray m_fogQuad;
        sf::Vector2f m_fogOffset;
        sf::Color m_fogColor;
        // sf::Vector2f m_fog;
        
        /**
         * @brief Function to draw all the fog and the light sources.
         */
        void draw(sf::RenderTarget&, sf::RenderStates)const override;
    public:
        
        /**
         * @brief List of segments that cast shadows.
         * @details Every LightSource added to this layer will add this 
         * segment pool pointer to its list.
         * @see LightSource::m_ptrSegmentPool
         */
        std::vector<Segment> m_segmentPool;
        
        /**
         * @brief Constructor.
         */
        Lighting();
        
        /**
         * @brief Copy constructor.
         * @param lighting Object to copy.
         */
        Lighting(const Lighting& lighting);
        
        /**
         * @brief Add a light source to the layer.
         * @param light Pointer to the LightSource.
         * 
         */
        void addLightSource(LightSource* light) ;
        
        /**
         * @brief Remove a light source from the layer.
         * @param light Pointer to the LightSource.
         */
        void removeLightSource(LightSource* light) ;
        
        /**
         * @brief Removes all lights.
         * @details This function doesn't delete the pointers, only
         * forgets them. Also, the segment pool is not deleted. To clear
         * it, you must explicitly clear m_segmentPool.
         */
        void clear() ;
        
        /**
         * @brief Set fog size.
         * @details Requires to update the fog.
         * @param width Fog width.
         * @param height Fog height.
         * @see adjustFog updateFog
         */
        void setFogSize(float width, float height);
        
        /**
         * @brief Set fog size.
         * @details Requires to update the fog.
         * @param size Fog size.
         * @see adjustFog updateFog
         */
        void setFogSize(sf::Vector2f size);
        
        /**
         * @brief Get size of the rectangle that is the fog.
         * @see setFogSize adjustFog
         */
        sf::Vector2f getFogSize() const;
        
        /**
         * @brief Set fog position.
         * @details Requires to update the fog.
         * @param x Fog position in horizontal axis.
         * @param y Fog position in vertical axis.
         * @see adjustFog updateFog
         */
        void setFogPosition(float x, float y);
        
        /**
         * @brief Set fog position.
         * @details Requires to update the fog.
         * @param position Fog position.
         * @see adjustFog updateFog
         */
        void setFogPosition(sf::Vector2f position);
        
        /**
         * @brief Get the position of the rectangle that is the fog.
         * @see setFogPosition adjustFog
         */
        sf::Vector2f getFogPosition() const;
        
        /**
         * @brief Set the fog position and size to match the view.
         * @details Requires to update the fog.
         * @param view View to adjust the fog.
         * @see updateFog setFogSize setFogPosition
         */
        void adjustFog(const sf::View& view);
        
        /**
         * @brief Set the fog position and size to match the view. 
         * @details The alpha value of the color is ignored. To change it. Requires to update the fog.
         * @param color Color to set the fog.
         * @see updateFog
         */
        void setFogColor(sf::Color color);
        
        /**
         * @brief Get the plain color of the fog.
         * @details The alpha value of the color returned is 255. To get 
         * the alpha value in a range between 0 and 1, use getFogOpacity.
         * @see setFogColor getFogOpacity
         */
        sf::Color getFogColor() const;
        
        /**
         * @brief Set the opacity of the fog.
         * @details Requires to update the fog.
         * @param color Value between 0 for total transparency and 1 for
         * total opacity.
         * @see updateFog
         */
        void setFogOpacity(float opacity);
        
        /**
         * @brief Get the fog color alpha value.
         * @see setFogOpacity getFogColor
         */
        float getFogOpacity() const;
        
        /**
         * @brief Applies fog changes and updates the field of view of 
         * the lights.
         * @details If the fog is visible and the lights move, this
         * function is required to keep the fog coherent with the lights.
         */
        void updateFog();
    };
}

#endif
