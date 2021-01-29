#ifndef __SFML_UTIL_GRAPHICS_VERTEXARRAY_HPP__
#define __SFML_UTIL_GRAPHICS_VERTEXARRAY_HPP__

#include "SFML/Graphics/VertexArray.hpp"

namespace sfu{
    void setColor(sf::VertexArray& va, const sf::Color& color);
    void transform(sf::VertexArray& va, const sf::Transform& t);
    void move(sf::VertexArray& va, const sf::Vector2f& d);
}

#endif
