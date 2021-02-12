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
     * @brief Object to manage ambiental light and fog.
     * @details
     * 
     * A LightingArea is a wrapper class to a sf::RenderTexture that provides
     * the required functions to use it as a mask for the light or a layer of
     * extra lighting. This behaviour is specified through the 
     * [operation mode](@ref LightingArea::Mode) of the area.
     * 
     * <table width="100%">
     * <tr>
     *  <td align="center"> <img src="lightingArea01.png" width="300px"> <br> <em>No LightingArea</em> </td>
     *  <td align="center"> <img src="lightingArea02.png" width="300px"> <br> <em>FOG mode (Color black, medium opacity)</em> </td>
     *  <td align="center"> <img src="lightingArea03.png" width="300px"> <br> <em>AMBIENTAL mode (Color yellow, low opacity)</em> </td>
     * </tr>
     * </table>
     * 
     * It can be used with a plain color or with another sf::Texture as base.
     * If this is the case, such texture must exist and be managed externally
     * during the life of the LightingArea. All changes made to the color,
     * opacity or texture of the area require a call to @ref clear and
     * @ref display to make effect.
     * 
     * <div align="center">
     * <img width="300px" src="lightingArea04.png"> <br> <em>LightingArea in FOG mode with texture</em>
     * </div>
     * 
     * As the sf::RenderTexture may be a heavy resource to be creating, there 
     * is only two moments in which you are able to do so. The first one and 
     * most common is upon construction, where you can specify the size of the
     * area, that will be the size of the sf::RenderTexture. The second one is
     * upon the assignation of a texture (with @ref setAreaTexture), on which
     * the area is created again to match the size of the new texture.
     * 
     * There are two things to note  about this:
     *   1. To change the size of a LightingArea that has already been
     * constructed, without changing the base texture, you have to scale it as
     * you would do with any other sf::Transformable.
     *   2. If you change the texture of the area, its size might also be 
     * modified. So, if you want to change the texture and the size, you must
     * change the texture first and scale it after that.
     * 
     */
    class LightingArea: public sf::Transformable, public sf::Drawable{
    public:
        /**
         * @brief Operation modes for a LightingArea.
         * @see setMode, getMode
         */
        enum Mode {
            /**
             * In this mode, the area behaves like a mask through which is only
             * possible to see by drawing light on it.
             */
            FOG,
            /**
             * Use the area as an extra layer of light.
             */
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
         * @details Constructs a LightingArea with plain color and specifies
         * the initial position and the size of the created sf::RenderTexture.
         * @param mode
         * @param position
         * @param size
         */
        LightingArea(Mode mode, const sf::Vector2f& position, const sf::Vector2f& size);
        
        /**
         * @brief Constructor.
         * @details Constructs a LightArea from a texture, in position {0, 0}.
         * As an optional parameter, you can pass the rectangle of the texture
         * that delimits the subsection of the texture to use.
         * @param mode
         * @param texture
         * @param rect
         */
        LightingArea(Mode mode, const sf::Texture* texture, sf::IntRect rect=sf::IntRect());
        
        /**
         * @brief Get the local bounding rectangle of the area.
         * @details The rectangle returned bounds the area before any 
         * transformations.
         * @returns Local bounding rectangle of the area.
         */
        sf::FloatRect getLocalBounds() const;
        
        /**
         * @brief Get the global bounding rectangle of the area.
         * @details The rectangle returned bounds the area with the
         * transformation already applied.
         * @returns Global bounding rectangle of the area.
         */
        sf::FloatRect getGlobalBounds() const;
        
        /**
         * @brief Set color of the fog/light. 
         * @details If the area has no texture, the plain color is used in
         * the next calls to @ref clear. Otherwise, the texture is multiplied
         * by the color. In both cases, the alpha value of the color is
         * preserved.
         * 
         * The default color is sf::Color::White.
         * @param color
         * @see getAreaColor, setAreaOpacity
         */
        void setAreaColor(sf::Color color);
        
        /**
         * @brief Get color of the fog/light.
         * @returns The plain color of the fog/light.
         * @see setAreaColor, setAreaOpacity
         */
        sf::Color getAreaColor() const;
        
        /**
         * @brief Set the opacity of the fog/light.
         * @details The opacity is a value multiplied to the alpha value before
         * any use of the color, to ease the separate manipulation.
         * @param opacity
         * @see getAreaOpacity, setAreaColor
         */
        void setAreaOpacity(float opacity);
        
        /**
         * @brief Get the opacity of the fog/light.
         * @returns The opacity of the fog/light.
         * @see setAreaOpacity getAreaColor
         */
        float getAreaOpacity() const;
        
        /**
         * @brief Set the texture of the fog/light.
         * @param texture Pointer to the new texture. Pass a null pointer to
         * just unset the texture.
         * @param rect Optional rectangle to call @ref setTextureRect. If none 
         * is specified, the whole texture is used.
         * @see getAreaTexture
         */
        void setAreaTexture(const sf::Texture* texture, sf::IntRect rect=sf::IntRect());
        
        /**
         * @brief Get the texture of the fog/light.
         * @returns Pointer to the texture of the fog/light.
         * @see getAreaTexture
         */
        const sf::Texture* getAreaTexture() const;
        
        /**
         * @brief Set the rectangle of the used sub-section of the texture.
         * @details Note that this function won't adjust the size of the area
         * to fit the new rectangle.
         * @param rect
         * @see getTextureRect
         */
        void setTextureRect(const sf::IntRect& rect);
        
        /**
         * @brief Get the rectangle of the used sub-section of the texture.
         * @returns The rectangle of the used sub-section of the texture.
         * @see setTextureRect
         */
        sf::IntRect getTextureRect() const;
        
        /**
         * @brief Set the lighting mode.
         * @param mode
         * @see LightingArea::Mode, setMode
         */
        void setMode(Mode mode);
        
        /**
         * @brief Set the lighting mode.
         * @returns The lighting mode.
         * @see LightingArea::Mode, setMode
         */
        Mode getMode() const;
        
        /**
         * @brief Updates and restores the color and the texture.
         * @details In FOG mode, it restores the covered areas.
         */
        void clear();
        
        /**
         * @brief In FOG mode, makes visible the area illuminated by the light.
         * @details In FOG mode with opacity greater than zero, this function.
         * is necessary to keep the lighting coherent. In AMBIENTAL mode, this
         * function has no effect. 
         * @param light
         */
        void draw(const LightSource& light);
        
        /**
         * @brief Calls display on the sf::RenderTexture.
         * @details Updates the changes made since the last call to @ref clear.
         */
        void display();
    };
}

#endif
