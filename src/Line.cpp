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
    
    Line::LineRelativePosition Line::intersection(const Line& l) const{
        float t1, t2;
        return intersection(l,t1,t2);
    }
    Line::LineRelativePosition Line::intersection(const Line& l, float& t1) const{
        float t2;
        return intersection(l,t1,t2);
    }
    Line::LineRelativePosition Line::intersection(const Line& l, float& t1, float& t2) const{
        auto& a = m_origin;
        auto& v = m_direction;
        auto& b = l.m_origin;
        auto& w = l.m_direction;
        
        float th = angle(v, w);
        if(th < 0.001f || th > 359.99f){
            if(relativePosition(a) == 0){
                return COINCIDENTIAL;
            }else{
                return PARALLEL;
            }
        }
        
        if(std::abs(w.y) < 0.001f){
            t1 = (b.y-a.y) / v.y;
            t2 = (a.x + t1*v.x - b.x) / w.x;
        }else{
            t1 = (w.y * (b.x-a.x) + w.x * (a.y-b.y)) / (v.x*w.y - v.y*w.x);
            t2 = (t1*v.y + a.y - b.y) / w.y;
        }
        
        return SECANT;
    }
    sf::Vector2f Line::point(float param) const{
        return m_origin + param*m_direction;
    }
}
