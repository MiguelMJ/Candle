#include "Candle/Lighting.hpp"
#include <iostream>
#include <iomanip>
#include <memory>
#include <ctime>

int main(){
    // === WINDOW ===
    float WIDTH = 700;
    float HEIGHT = 700;
    sf::RenderWindow w(sf::VideoMode(WIDTH,HEIGHT), "Candle - demo");
    w.setVerticalSyncEnabled(true);
    
    // === LIGHTING ===
    candle::Lighting ll;
    ll.adjustFog(w.getView());
    
    // === BACKGROUND ===
    int rowNum = 16; 
    int colNum = 16;
    sf::Color bgc[3] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};
    sf::VertexArray bg(sf::Quads, colNum*rowNum*4);
    float cellWidth = WIDTH/colNum;
    float cellHeight = HEIGHT/rowNum;
    for(int i=0; i < colNum*rowNum; i++){
        int p = i*4;
        int pp = p+1;
        int ppp = p+2;
        int pppp = p+3;
        float x, y;
        x = cellWidth * (i%colNum);
        y = cellHeight * (i/colNum); // integer division, don't simplify
        bg[p].color = bg[pp].color = bg[ppp].color = bg[pppp].color = bgc[i%3];
        bg[p].position = sf::Vector2f(x,y);
        bg[pp].position = sf::Vector2f(x,y+cellHeight);
        bg[ppp].position = sf::Vector2f(x+cellWidth,y+cellHeight);
        bg[pppp].position = sf::Vector2f(x+cellWidth,y);
    }
    
    // === INTERACTIVITY===
    sf::Color lightColors[4] = {sf::Color::White, 
                                sf::Color::Magenta, 
                                sf::Color::Yellow,
                                sf::Color::Cyan };
    sf::VertexArray segmentLines(sf::Lines, 0);
    std::vector<std::unique_ptr<candle::LightSource>> lights;
    
    float blockWidth = 20;
    float blockHeight = 20;
    sf::RectangleShape mouseBlock(sf::Vector2f(blockWidth*2, blockHeight*2));
    mouseBlock.setOrigin(blockWidth,blockHeight);
    mouseBlock.setFillColor(sf::Color::Transparent);
    mouseBlock.setOutlineThickness(1);
    candle::LightSource mouseLight;
    bool lightOrBlock = true;
    ll.addLightSource(&mouseLight);
    int color = 0;
    float mouseLightRadius = 100;
    float mouseLightIntensity = 1.0;
    float fogOpacity = 1.0;
    mouseLight.setRadius(mouseLightRadius);
    mouseLight.setIntensity(mouseLightIntensity);
    ll.setFogOpacity(fogOpacity);
    sf::Time dt;
    
    // === FUNCTIONS ===
    auto info = [&](){
        std::cout << "\r";
        std::cout << "Bloques: " << std::setw(3) << ll.m_segmentPool.size()/4;
        std::cout << " - Luces: " << std::setw(3) << lights.size();
        std::cout << " - " << std::setw(8) << dt.asMilliseconds() << "ms";
    };
    auto removeMouseBlockSegments = [&](){
        if(!ll.m_segmentPool.empty()){
            for(int i=0; i<4; i++){
                ll.m_segmentPool.pop_back();
            }
        }
    };
    auto addMouseBlockSegments = [&](){
        auto mbs = candle::segments(mouseBlock.getGlobalBounds());
        ll.m_segmentPool.insert(ll.m_segmentPool.end(),mbs.begin(), mbs.end());
    };
    auto castAllLights = [&](){
        for(auto &l : lights){
            l->castLight();
        }
    };
    auto updateCastOnMouseMove = [&](){
        auto mpi = sf::Mouse::getPosition(w);
        sf::Vector2f mp(mpi.x, mpi.y);
        mouseLight.setPosition(mpi.x,mpi.y);
        mouseBlock.setPosition(mpi.x,mpi.y);
        if(lightOrBlock){
            mouseLight.castLight();
        }else{
            removeMouseBlockSegments();
            addMouseBlockSegments();
            castAllLights();
        }
    };
    auto putLightOrBlock = [&](){
        if(lightOrBlock){
            auto lptr = new candle::LightSource(mouseLight);
            lights.emplace_back(lptr);
            ll.addLightSource(lptr);
            mouseLight.castLight();
        }else{
            auto mpi = sf::Mouse::getPosition(w);
            sf::Vector2f mp(mpi.x, mpi.y);
            candle::Segment t = {{mp.x-blockWidth,mp.y-blockHeight}, {mp.x+blockWidth, mp.y-blockHeight}};
            candle::Segment l = {{mp.x+blockWidth,mp.y-blockHeight}, {mp.x+blockWidth, mp.y+blockHeight}};
            candle::Segment b = {{mp.x+blockWidth,mp.y+blockHeight}, {mp.x-blockWidth, mp.y+blockHeight}};
            candle::Segment r = {{mp.x-blockWidth,mp.y+blockHeight}, {mp.x-blockWidth, mp.y-blockHeight}};
            ll.m_segmentPool.push_back(t);
            ll.m_segmentPool.push_back(l);
            ll.m_segmentPool.push_back(b);
            ll.m_segmentPool.push_back(r);
            segmentLines.append(sf::Vertex(t.first, sf::Color::White));
            segmentLines.append(sf::Vertex(t.second, sf::Color::White));
            segmentLines.append(sf::Vertex(l.first, sf::Color::White));
            segmentLines.append(sf::Vertex(l.second, sf::Color::White));
            segmentLines.append(sf::Vertex(b.first, sf::Color::White));
            segmentLines.append(sf::Vertex(b.second, sf::Color::White));
            segmentLines.append(sf::Vertex(r.first, sf::Color::White));
            segmentLines.append(sf::Vertex(r.second, sf::Color::White));
            castAllLights();
        }
    };
    auto cleanScreen = [&](){
        ll.clear(); // clear light pointers
        ll.m_segmentPool.clear(); // clear logic segments
        lights.clear(); // clear lights
        segmentLines.clear(); // clear visible segments
        if(lightOrBlock) ll.addLightSource(&mouseLight);
    };
    auto saveCapture = [&](){
        sf::Texture tex;
        tex.create(w.getSize().x, w.getSize().y);
        tex.update(w);
        std::string name = "candle-capture-";
        char timestr[13];
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timestr,13,"%y%m%d%H%M%S",timeinfo);
        name += timestr;
        name += ".png";
        if(!tex.copyToImage().saveToFile(name)){
            exit(1);
        }
    };
    
    // === INITIALIZATION ===
    mouseLight.setColor(lightColors[color]);
    sf::Clock clock;
    // === MAIN LOOP ===
    while(w.isOpen()){
        sf::Event e;
        while(w.pollEvent(e)){
            switch(e.type){
                case sf::Event::Closed:
                    w.close();
                    break;
                case sf::Event::MouseMoved:
                        updateCastOnMouseMove();
                    break;
                case sf::Event::MouseWheelScrolled:
                    mouseLightRadius += 100*e.mouseWheelScroll.delta/3;
                    mouseLight.setRadius(mouseLightRadius);
                    mouseLight.castLight();
                    break;
                case sf::Event::MouseButtonPressed:
                    if(e.mouseButton.button == sf::Mouse::Left){
                            putLightOrBlock();
                    }else if(e.mouseButton.button == sf::Mouse::Right){
                        lightOrBlock = !lightOrBlock;
                        if(lightOrBlock){
                            ll.addLightSource(&mouseLight);
                            removeMouseBlockSegments();
                        }else{
                            ll.removeLightSource(&mouseLight);
                            addMouseBlockSegments();
                        }
                        castAllLights();
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(e.key.code == sf::Keyboard::Space){
                        cleanScreen();
                    }else if(e.key.code == sf::Keyboard::C){
                        mouseLight.setColor(lightColors[(++color)%4]);
                        mouseLight.castLight();
                    }else if(e.key.code == sf::Keyboard::A){
                        if(lightOrBlock && mouseLightIntensity > 0){
                            mouseLightIntensity-=0.05;
                            mouseLight.setIntensity(mouseLightIntensity);
                            mouseLight.castLight();
                        }
                    }else if(e.key.code == sf::Keyboard::S){
                        if(lightOrBlock && mouseLightIntensity < 1){
                            mouseLightIntensity+=0.05;
                            mouseLight.setIntensity(mouseLightIntensity);
                            mouseLight.castLight();
                        }
                    }else if(e.key.code == sf::Keyboard::G){
                        mouseLight.setGlow(!mouseLight.getGlow());
                    }else if(e.key.code == sf::Keyboard::Z){
                        if(fogOpacity > 0){
                            fogOpacity-=0.05;
                            ll.setFogOpacity(fogOpacity);
                        }
                    }else if(e.key.code == sf::Keyboard::X){
                        if(fogOpacity < 1){
                            fogOpacity+=0.05;
                            ll.setFogOpacity(fogOpacity);
                        }
                    }else if(e.key.code == sf::Keyboard::P){
                        saveCapture();
                    }
                    break;
                default:
                    break;
            }
        }
        
        // Update fog
        ll.updateFog();
        // Print info
        info();
        // Draw
        w.clear();
        
        w.draw(bg);
        w.draw(ll);
        if(!lightOrBlock){
            w.draw(mouseBlock);
        }
        w.draw(segmentLines);
        
        w.display();
        dt = clock.restart();
    }
    std::cout << std::endl;
    return 0;
}
