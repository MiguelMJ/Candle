#ifndef __SFML_UTIL_GEOMETRY_LINE_HPP__
#define __SFML_UTIL_GEOMETRY_LINE_HPP__

#include <cmath>

#include <SFML/System/Vector2.hpp>

#include "sfml-util/geometry/Vector2.hpp"

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
    template <typename Iterator>
    sf::Vector2f castRay(const Iterator& begin, const Iterator& end, Line ray, float maxRange=std::numeric_limits<float>::infinity()){
        float minRange = maxRange;
        ray.m_direction = sfu::normalize(ray.m_direction);
        for(auto it = begin; it != end; it++){
            float t_seg, t_ray;
            if(
                it -> intersection(ray, t_seg, t_ray) == sfu::Line::SECANT
                && t_ray <= minRange
                && t_ray >= 0.f
                && t_seg <= 1.f
                && t_seg >= 0.f
            ){
                minRange = t_ray;
            }
        }
        return ray.point(minRange);
    }
}

#endif
