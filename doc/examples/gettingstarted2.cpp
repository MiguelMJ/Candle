#include <SFML/Graphics.hpp>
#include "Candle/RadialLight.hpp"
#include "Candle/LightingArea.hpp"

int main(){
    // create window
    sf::RenderWindow w(sf::VideoMode(300, 379), "app");
    
    // create a light source
    candle::RadialLight light;
    light.setRange(100);
    light.setFade(false);
    
    // load the image
    sf::Texture img1;
    if(!img1.loadFromFile("sunflowers_van_gogh.png")){
        exit(1);
    }
    sf::Sprite background(img1);
    
    // create the lighting area
    candle::LightingArea fog(candle::LightingArea::FOG,
                             sf::Vector2f(0.f, 0.f),
                             sf::Vector2f(300.f, 379.f));
    fog.setAreaColor(sf::Color::Black);
    
    // main loop
    while(w.isOpen()){
        sf::Event e;
        while(w.pollEvent(e)){
            if(e.type == sf::Event::Closed){
                w.close();
            }else if(e.type == sf::Event::MouseMoved){
                sf::Vector2f mp(sf::Mouse::getPosition(w));
                light.setPosition(mp);
            }
        }
        
        fog.clear();
        fog.draw(light);
        fog.display();
        
        w.clear();
        w.draw(background);
        w.draw(fog);
        w.display();
    }
    return 0;
}
