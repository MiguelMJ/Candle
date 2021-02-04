#include "sfml-util/graphics/VertexArray.hpp"
#include "sfml-util/graphics/Color.hpp"

namespace sfu{
    void setColor(sf::VertexArray& va, const sf::Color& color){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].color = color;
        }
    }
    
    void transform(sf::VertexArray& va, const sf::Transform& t){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].position = t.transformPoint(va[i].position);
        }
    }
    
    void move(sf::VertexArray& va, const sf::Vector2f& d){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].position += d;
        }
    }
    
    void darken(sf::VertexArray& va, float r){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].color = darken(va[i].color, r);
        }
    }
    
    void lighten(sf::VertexArray& va, float r){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].color = lighten(va[i].color, r);
        }
    }
    
    void interpolate(sf::VertexArray& va, const sf::Color& c, float r){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].color = interpolate(va[i].color, c, r);
        }
    }
    
    void complementary(sf::VertexArray& va){
        for(size_t i = 0; i < va.getVertexCount(); i++){
            va[i].color = complementary(va[i].color);
        }
    }
    
}
