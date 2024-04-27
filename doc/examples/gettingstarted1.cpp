#include <SFML/Graphics.hpp>
#include "Candle/PointLight.hpp"

int main(){
    // create window
    sf::RenderWindow w(sf::VideoMode(400, 400), "app");
    
    // create a light source
    candle::PointLight light;
    light.setRange(150);
    
    // create an edge pool
    candle::EdgeVector edges;
    edges.emplace_back(sf::Vector2f(200.f, 100.f), 
                       sf::Vector2f(200.f, 300.f));
    
    // main loop
    while(w.isOpen()){
        sf::Event e;
        while(w.pollEvent(e)){
            if(e.type == sf::Event::Closed){
                w.close();
            }else if(e.type == sf::Event::MouseMoved){
                sf::Vector2f mp(sf::Mouse::getPosition(w));
                light.setPosition(mp);
            	light.castLight(edges.begin(), edges.end());
            }
        }
        
        w.clear();
        w.draw(light);
        w.display();
    }
    return 0;
}
