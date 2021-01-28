#ifndef __SFML_UTIL_GEOMETRY_VECTOR2_HPP__
#define __SFML_UTIL_GEOMETRY_VECTOR2_HPP__

#include <cmath>

#include <SFML/System/Vector2.hpp>

namespace sfu{
    template <typename T>
    float magnitude(const sf::Vector2<T>& v){
        return std::sqrt(v.x*v.x + v.y*v.y);
    }
    template <typename T>
    float magnitude2(const sf::Vector2<T>& v){
        return v.x*v.x + v.y*v.y;
    }
    template <typename T>
    sf::Vector2<T> normalize(const sf::Vector2<T>& v){
        float m = magnitude(v);
        return sf::Vector2<T>(v.x/m, v.y/m);
    }
    template <typename T>
    T dot(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2){
        return v1.x*v2.x + v1.y*v2.y;
    }
    template <typename T>
    float angle(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2){
        return std::acos(dot(v1,v2)/(magnitude(v1)*magnitude(v2))) * 180/M_PI;
    }
    template <typename T>
    float angle(const sf::Vector2<T>& v){
        return fmod(std::atan2(v.y, v.x) * 180.f/M_PI + 360.f, 360.f);
    }
}

#endif
