#include "sfml-util/graphics/Color.hpp"

namespace sfu{
    sf::Color darken(const sf::Color& c, float r){
        return sf::Color(c.r * (1.f-r), c.g * (1.f-r), c.b * (1.f-r), c.a);
    }
    
    sf::Color lighten(const sf::Color& c, float r){
        return sf::Color(c.r * (1.f+r), c.g * (1.f+r), c.b * (1.f+r), c.a);
    }
    
    sf::Color interpolate(const sf::Color& c1, const sf::Color& c2, float r){
        return sf::Color(c1.r + (c2.r - c1.r) * r, c1.g + (c2.g - c1.g) * r, c1.b + (c2.b - c1.b) * r, c1.a + (c2.a - c1.a) * r);
    }
    
    sf::Color complementary(const sf::Color& c){
        return sf::Color(255-c.r, 255-c.g, 255-c.b, c.a);
    }
    
}
