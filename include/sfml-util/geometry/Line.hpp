#ifndef __SFML_UTIL_GEOMETRY_LINE_HPP__
#define __SFML_UTIL_GEOMETRY_LINE_HPP__

#include <SFML/System/Vector2.hpp>

namespace sfu{
    struct Line{
        enum LineRelativePosition{
            PARALLEL,
            SECANT,
            COINCIDENTIAL
        };
        sf::Vector2f m_origin;
        sf::Vector2f m_direction;
        Line(const sf::Vector2f& p1, const sf::Vector2f& p2);
        Line(const sf::Vector2f& p, float angle);
        int relativePosition(const sf::Vector2f& point) const;
        float distance(const sf::Vector2f& point) const;
        LineRelativePosition intersection(const Line& l) const;
        LineRelativePosition intersection(const Line& l, float& t) const;
        LineRelativePosition intersection(const Line& l, float& t1, float& t2) const;
        sf::Vector2f point(float param) const;
    };
}

#endif
