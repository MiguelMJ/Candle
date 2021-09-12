/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains the Line struct and main raycast algorithm.
 */
#ifndef __SFML_UTIL_GEOMETRY_LINE_HPP__
#define __SFML_UTIL_GEOMETRY_LINE_HPP__

#include <limits>

#include <SFML/System/Vector2.hpp>

#include "Candle/geometry/Vector2.hpp"
#include "Candle/Constants.hpp"

namespace sfu{
    /**
     * @brief 2D %Line defined by an origin point and a direction vector.
     */
    struct Line{
        /**
         * Possible positions between two lines.
         */
        enum LineRelativePosition{
            PARALLEL, ///< They have no points in common.
            SECANT,  ///< They share one point.
            COINCIDENTIAL  ///< All of their points are in common
        };
        sf::Vector2f m_origin; ///< Origin point of the line.
        sf::Vector2f m_direction; ///< Direction vector (not necessarily  normalized)
        
        /**
         * @brief Construct a line that passes through @p p1 and @p p2
         * @details The direction is interpreted as p2 - p1.
         * @param p1 First point
         * @param p2 Second point
         */
        Line(const sf::Vector2f& p1, const sf::Vector2f& p2);
        
        /**
         * @brief Construct a line defined by a point and an angle.
         * @details The direction is interpreted as {cos(angle), sin(angle)}.
         * @param p Origin point
         * @param angle Angle defining the line
         */
        Line(const sf::Vector2f& p, float angle);
        
        /**
         * @brief Return the relative position of a point to the line.
         * @details If the point is to the right of the direction vector, the
         * value returned is -1. If it is to the left, it is +1. If the point
         * belongs to the line, returns 0.
         * @param point
         * @returns -1, 0 or 1
         */
        int relativePosition(const sf::Vector2f& point) const;
        
        /**
         * @brief Return the minimum distance of a point to the line.
         * @param point
         * @returns The minimum distance of a point to the line.
         */
        float distance(const sf::Vector2f& point) const;
        
        /**
         * @brief Returns the relative position of one line to another.
         * @param line
         * @returns The relative position of the two lines.
         */
        LineRelativePosition intersection(const Line& line) const;
        
        /**
         * @brief Returns the relative position of one line to another.
         * @details If the relative position is SECANT, the output argument
         * @p t contains the parameter required to get the intersection
         * point with **this** line.
         * @param line
         * @param t (Output argument)
         * @returns The relative position of the two lines.
         * @see point
         */
        LineRelativePosition intersection(const Line& line, float& t) const;
        
        /**
         * @brief Returns the relative position of one line to another.
         * @details If the relative position is SECANT, the output argument
         * @p t1 contains the parameter required to get the intersection
         * point with **this** line and @p t2, the parameter required to 
         * get it with @p line.
         * @param line
         * @param t1 (Output argument)
         * @param t2 (Output argument)
         * @returns The relative position of the two lines.
         * @see point
         */
        LineRelativePosition intersection(const Line& line, float& t1, float& t2) const;
        
        /**
         * @brief Get a point of the line.
         * @details The point is obtained is m_origin + param * m_direction
         * @param param
         * @returns A point of the line
         */
        sf::Vector2f point(float param) const;
        
    };
    
    /**
     * @brief Cast a ray against a set of segments.
     * @details Use a line as a ray, casted from its 
     * [origin](@ref sfu::Line::m_origin) in its 
     * [direction](@ref sfu::Line::m_direction). It is intersected with * a set
     * of segments, represented as [Lines](sfu::Line) too, and the one closest
     * to the cast point is returned.
     * 
     * Segments are interpreted to be delimited by the @p ray.m_origin and
     * @p ray.point(1).
     * 
     * @param begin Iterator to the first ray.
     * @param end Iterator to the last ray.
     * @param ray
     * @param maxRange Optional argument to indicate the max distance allowed
     * for a ray to hit a segment.
     */
    template <typename Iterator>
    sf::Vector2f castRay(const Iterator& begin,
                         const Iterator& end,
                         Line ray, 
                         float maxRange=std::numeric_limits<float>::infinity()){
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
