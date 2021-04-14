/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains utility functions for 2D vectors.
 */
#ifndef __SFML_UTIL_GEOMETRY_VECTOR2_HPP__
#define __SFML_UTIL_GEOMETRY_VECTOR2_HPP__

#include <cmath>

#include <SFML/System/Vector2.hpp>

#include "Candle/Constants.hpp"

namespace sfu{
    /**
     * Get the magnitude of a 2D vector.
     */
    template <typename T>
    float magnitude(const sf::Vector2<T>& v){
        return std::sqrt(v.x*v.x + v.y*v.y);
    }
    
    /**
     * Get the squared magnitude of a 2D vector.
     */
    template <typename T>
    float magnitude2(const sf::Vector2<T>& v){
        return v.x*v.x + v.y*v.y;
    }
    
    /**
     * Get the normalized version of a 2D vector.
     */
    template <typename T>
    sf::Vector2<T> normalize(const sf::Vector2<T>& v){
        float m = magnitude(v);
        return sf::Vector2<T>(v.x/m, v.y/m);
    }
    
    /**
     * Get the dot product of two 2D vectors.
     */
    template <typename T, typename Q>
    T dot(const sf::Vector2<T>& v1, const sf::Vector2<Q>& v2){
        return v1.x*v2.x + v1.y*v2.y;
    }
    
    /**
     * Get the angle between two 2D vectors.
     */
    template <typename T, typename Q>
    float angle(const sf::Vector2<T>& v1, const sf::Vector2<Q>& v2){
        return std::acos(dot(v1,v2)/(magnitude(v1)*magnitude(v2))) * 180/sfu::PI;
    }
    
    /**
     * Get the angle of a 2D vector with the X axis.
     */
    template <typename T>
    float angle(const sf::Vector2<T>& v){
        return fmod(std::atan2(v.y, v.x) * 180.f/sfu::PI + 360.f, 360.f);
    }
}

#endif
