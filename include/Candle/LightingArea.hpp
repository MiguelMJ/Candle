/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the LightingArea class.
 */
#ifndef __CANDLE_LIGHTING_HPP__
#define __CANDLE_LIGHTING_HPP__

#include <set>

#include "SFML/Graphics.hpp"

#include "sfml-util/geometry/Line.hpp"
#include "Candle/LightSource.hpp"

namespace candle{
    /**
     * @brief Object to manage light, fog and shadows in an area.
     * @details ...
     */
    class LightingArea: public sf::Transformable, public sf::Drawable{
    public:
        enum Mode {
            FOG,
            AMBIENTAL
        };
    private:
        const sf::Texture* m_baseTexture;
        sf::IntRect m_baseTextureRect;
        sf::VertexArray m_baseTextureQuad;
        sf::RenderTexture m_renderTexture;
        sf::VertexArray m_areaQuad;
        sf::Color m_color;
        float m_opacity;
        sf::Vector2f m_size;
        Mode m_mode;
        /**
         * @brief Function to draw all the fog and the light sources.
         */
        void draw(sf::RenderTarget&, sf::RenderStates)const override;
        sf::Color getActualColor() const;
        void initializeRenderTexture(const sf::Vector2f& size);
    public:
        
        /**
         * @brief Constructor.
         */
        LightingArea(Mode mode, const sf::Vector2f& position, const sf::Vector2f& size);
        
        /**
         * @brief Constructor.
         */
        LightingArea(Mode mode, const sf::Texture* texture, sf::IntRect rect=sf::IntRect());
        
        /**
         * @brief Get the local bounding rectangle of the area.
         */
        sf::FloatRect getLocalBounds() const;
        
        /**
         * @brief Get the global bounding rectangle of the area.
         */
        sf::FloatRect getGlobalBounds() const;
        
        /**
         * @brief Set the fog position and size to match the view. 
         * @details The alpha value of the color is ignored. To change it. May require to update the fog.
         * @param color Color to set the fog.
         * @see update
         */
        void setAreaColor(sf::Color color);
        
        /**
         * @brief Get the plain color of the fog.
         * @details The alpha value of the color returned is 255. To get 
         * the alpha value in a range between 0 and 1, use getAreaOpacity.
         * @see setAreaColor getAreaOpacity
         */
        sf::Color getAreaColor() const;
        
        /**
         * @brief Set the opacity of the fog.
         * @details May require to update the fog.
         * @param color Value between 0 for total transparency and 1 for
         * total opacity.
         * @see update
         */
        void setAreaOpacity(float opacity);
        
        /**
         * @brief Get the fog color alpha value.
         * @see setAreaOpacity getAreaColor
         */
        float getAreaOpacity() const;
        
        /**
         * @brief Set the texture of the fog or ambiental light.
         * @param texture Pointer to the new texture.
         * @see getAreaTexture
         */
        void setAreaTexture(const sf::Texture* texture, sf::IntRect rect=sf::IntRect());
        
        /**
         * @brief Get the texture of the fog or ambiental light.
         * @see getAreaTexture
         */
        sf::Texture* getAreaTexture() const;
        
        /**
         * @brief Set the sub-rectangle of the texture for the area.
         */
        void setTextureRect(const sf::IntRect& rect);
        
        /**
         * @brief Get the sub-rectangle of the texture for the area.
         */
        sf::IntRect getTextureRect() const;
        
        /**
         * @brief Set the lighting mode
         */
        void setMode(Mode mode);
        
        /**
         * @brief Set the lighting mode
         */
        Mode getMode() const;
        
        /**
         * @brief In FOG mode restores the covered areas.
         */
        void clear();
        
        /**
         * @brief Applies the effect of light to the fog in FOG mode.
         * @details In FOG mode with opacity greater than zero, this function.
         * is necessary to keep the lighting coherent.
         */
        void draw(const LightSource& light);
        
        /**
         * @brief Display changes made to the fog
         */
        void display();
    };
}

#endif
