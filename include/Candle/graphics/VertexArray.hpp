#ifndef __SFML_UTIL_GRAPHICS_VERTEXARRAY_HPP__
#define __SFML_UTIL_GRAPHICS_VERTEXARRAY_HPP__

#include "SFML/Graphics/VertexArray.hpp"

namespace sfu{
    void setColor(sf::VertexArray& va, const sf::Color& color);
    void transform(sf::VertexArray& va, const sf::Transform& t);
    void move(sf::VertexArray& va, const sf::Vector2f& d);
    void darken(sf::VertexArray& va, float r);
    void lighten(sf::VertexArray& va, float r);
    void interpolate(sf::VertexArray& va, const sf::Color& c, float r);
    void complementary(sf::VertexArray& va);
}

#endif
