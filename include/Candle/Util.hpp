/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains utility functions and structuresfor the 
 * lighting algorithm.
 */
#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <utility>
#include <array>
#include <vector>

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/VertexArray.hpp"

namespace candle{
    static const float PI = 3.14159265f;
    static const float PI2 = PI*2;

    void move(sf::VertexArray& va, sf::Vector2f);    
    
    typedef std::pair<sf::Vector2f, sf::Vector2f> Segment;
    typedef std::vector<sf::Vector2f> Polygon;
    typedef struct{
        sf::Vector2f origin;
        sf::Vector2f direction;
        float angle; // rads
    } Ray;
    
    Ray make_ray(sf::Vector2f p1, sf::Vector2f p2);
    Ray make_ray(sf::Vector2f orig, float ang); // rads
    Ray make_ray(Segment s);
    std::array<Segment, 4> segments(sf::FloatRect rect);
    
    std::pair<float, float> intersection(Ray r1, Ray r2);
    float deg2rad(float deg);
}

#endif

