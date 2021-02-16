#ifndef __SFML_UTIL_GRAPHICS_COLOR_HPP__
#define __SFML_UTIL_GRAPHICS_COLOR_HPP__

#include <SFML/Graphics/Color.hpp>

namespace sfu{
    sf::Color darken(const sf::Color& c, float r);
    sf::Color lighten(const sf::Color& c, float r);
    sf::Color interpolate(const sf::Color& c1, const sf::Color& c2, float r);
    sf::Color complementary(const sf::Color& c);
}

#endif
