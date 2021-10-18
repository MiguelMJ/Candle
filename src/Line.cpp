#include <limits>

#include "Candle/geometry/Line.hpp"
#include "Candle/geometry/Vector2.hpp"

namespace sfu{
    Line::Line(const sf::Vector2f& p1, const sf::Vector2f& p2):
        m_origin(p1),
        m_direction(p2 - p1){}

    Line::Line(const sf::Vector2f& p, float angle):
        m_origin(p)
        {
            const auto PI2 = sfu::PI*2;
            float ang = (float)fmod(angle*sfu::PI/180.f + sfu::PI , PI2);
            if(ang < 0) ang += PI2;
            ang -= sfu::PI;
            m_direction = {std::cos(ang), std::sin(ang)};
        }

    sf::FloatRect Line::getGlobalBounds() const{
        const sf::Vector2f& point1 = m_origin;
        sf::Vector2f point2 = m_direction + m_origin;

        //Make sure that the rectangle begin from the upper left corner
        sf::FloatRect rect;
        rect.left = (point1.x < point2.x) ? point1.x : point2.x;
        rect.top = (point1.y < point2.y) ? point1.y : point2.y;
        rect.width = std::abs(m_direction.x) + 1.0f; //The +1 is here to avoid having a width of zero
        rect.height = std::abs(m_direction.y) + 1.0f; //(SFML doesn't like 0 in rect)

        return rect;
    }

    int Line::relativePosition(const sf::Vector2f& point) const{
        float f = (point.x-m_origin.x) / m_direction.x - (point.y-m_origin.y) / m_direction.y;
        return (0.f < f) - (f < 0.f);
    }

    float Line::distance(const sf::Vector2f& point) const{
        float d;
        if(m_direction.x == 0){
            d = std::abs(point.x - m_origin.x);
        }else if(m_direction.y == 0){
            d = std::abs(point.y - m_origin.y);
        }else{
            float A = 1.f / m_direction.x;
            float B = - 1.f / m_direction.y;
            float C = - B*m_origin.y - A*m_origin.x;
            d = std::abs(A*point.x + B*point.y + C) / std::sqrt(A*A + B*B);
        }
        return d;
    }

    bool Line::intersection(const Line& lineB) const{
        float normA,normB;
        return intersection(lineB, normA,normB);
    }
    bool Line::intersection(const Line& lineB, float& normA) const{
        float normB;
        return intersection(lineB, normA,normB);
    }
    bool Line::intersection(const Line& lineB, float& normA, float& normB) const{
        const sf::Vector2f& lineAorigin = m_origin;
        const sf::Vector2f& lineAdirection = m_direction;
        const sf::Vector2f& lineBorigin = lineB.m_origin;
        const sf::Vector2f& lineBdirection = lineB.m_direction;

        //When the lines are parallel, we consider that there is not intersection.
        float lineAngle = angle(lineAdirection, lineBdirection);
        if( (lineAngle < 0.001f || lineAngle > 359.999f) || ((lineAngle < 180.001f) && (lineAngle > 179.999f)) ){
            return false;
        }

        //Math resolving, you can find more information here : https://ncase.me/sight-and-light/
        if ( (std::abs(lineAdirection.y) < 0.001f) || (std::abs(lineAdirection.x) > 0.0f) )
        {
            normB = (lineAdirection.x*(lineAorigin.y-lineBorigin.y) + lineAdirection.y*(lineBorigin.x-lineAorigin.x))/(lineBdirection.y*lineAdirection.x - lineBdirection.x*lineAdirection.y);
            normA = (lineBorigin.x+lineBdirection.x*normB-lineAorigin.x)/lineAdirection.x;
        }
        else
        {
            normA = (lineBdirection.x*(lineBorigin.y-lineAorigin.y) + lineBdirection.y*(lineAorigin.x-lineBorigin.x))/(lineAdirection.y*lineBdirection.x - lineAdirection.x*lineBdirection.y);
            normB = (lineAorigin.x+lineAdirection.x*normA-lineBorigin.x)/lineBdirection.x;
        }

        //Make sure that there is actually an intersection
        if ( (normB>0) && (normA>0) && (normA<sfu::magnitude(m_direction)) )
        {
            return true;
        }

        return false;
    }

    sf::Vector2f Line::point(float param) const{
        return m_origin + param*m_direction;
    }
}
