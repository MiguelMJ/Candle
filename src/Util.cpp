#include <cmath>
#include "Candle/Util.hpp"

namespace candle{
    void move(sf::VertexArray& va, sf::Vector2f d){
        for(unsigned i=0; i < va.getVertexCount(); i++){
            va[i].position.y += d.y;
            va[i].position.x += d.x;
        }
    }
    std::array<Segment, 4> segments(sf::FloatRect rect){
        sf::Vector2f tl(rect.left, rect.top);
        sf::Vector2f tr(rect.left + rect.width, rect.top);
        sf::Vector2f br(rect.left + rect.width, rect.top + rect.height);
        sf::Vector2f bl(rect.left, rect.top + rect.height);
        Segment top = {tl,tr};
        Segment right = {tr,br};
        Segment bottom = {br,bl};
        Segment left = {bl,tl};
        return {
            top,
            right,
            bottom,
            left
        };
    }float deg2rad(float deg){
        return deg * PI / 180;
    }
    Ray make_ray(sf::Vector2f p1, sf::Vector2f p2){
        sf::Vector2f d = p2 - p1;
        return {
            p1,
            d,
            std::atan2(d.y, d.x)
        };
    }
    Ray make_ray(sf::Vector2f orig, float ang){ // rads
        ang = (float)fmod(ang + PI , PI2);
        if(ang < 0) ang += PI2;
        ang -= PI;
        return {
            orig,
            sf::Vector2f(std::cos(ang), std::sin(ang)),
            ang
        };
    }
    Ray make_ray(Segment s){
        return make_ray(s.first, s.second);
    }
    std::pair<float, float> intersection(Ray r1, Ray r2){
        float t1, t2;
        t1 = t2 = std::numeric_limits<float>::infinity();
        sf::Vector2f dir1 (r1.direction), dir2 (r2.direction);
        sf::Vector2f orig1 (r1.origin), orig2 (r2.origin);
        if(dir1.x/dir2.x != dir1.y/dir2.y){
            t2 = (dir1.x * (orig2.y - orig1.y) + dir1.y * (orig1.x - orig2.x)) /
                 (dir2.x * dir1.y - dir2.y * dir1.x);
            t1 = (orig2.x + dir2.x * t2 - orig1.x) / dir1.x;
        }
        return std::make_pair(t1, t2);
    }
    std::pair<float, float> intersection(Segment s1, Segment s2){
        return intersection(make_ray(s1), make_ray(s2));
    }
    
}
