#include "sfml-util/graphics/VertexArray.hpp"

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
}
