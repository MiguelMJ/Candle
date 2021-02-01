#include <iostream>
#include <iomanip>
#include <memory>
#include <ctime>
#include <cmath>

#include "sfml-util/geometry/Polygon.hpp"

#include "Candle/Lighting.hpp"
#include "Candle/LightSource.hpp"
#include "Candle/RadialLight.hpp"
#include "Candle/DirectedLight.hpp"

/*
 * WINDOW
 */
const float WIDTH = 700;
const float HEIGHT = 700;
sf::RenderWindow w(sf::VideoMode(WIDTH, HEIGHT), "Candle - demo");

/*
 * LIGHTING
 */
candle::Lighting lighting;
std::vector<std::unique_ptr<candle::LightSource>> lights;
sf::VertexArray segmentVertices(sf::Lines, 0);

/*
 * BACKGROUND
 */
const int ROWS = 16;
const int COLS = 16;
float CELL_W = WIDTH / COLS;
float CELL_H = HEIGHT / ROWS;
const sf::Color BG_COLORS[] = {
    sf::Color::Red,
    sf::Color::Green,
    sf::Color::Blue};
sf::VertexArray background(sf::Quads, ROWS * COLS * 4);

/*
 * INTERACTIVITY - Brushes - Lights
 */
enum Brush{
    NONE = -1,
    RADIAL = 0,
    DIRECTED = 1,
    BLOCK = 2,
    LINE = 3};
Brush brush;
bool lineStarted;

const sf::Color L_COLORS[] = {
    sf::Color::White,
    sf::Color::Magenta,
    sf::Color::Yellow,
    sf::Color::Cyan};

sf::VertexArray mouseBlock(sf::Lines, 8);
float blockSize;

candle::RadialLight radialLight;
candle::DirectedLight directedLight;
bool control, shift, alt;

/*
 * INTERACTIVITY - Menu
 */
struct Button: public sf::RectangleShape{
    void (*function)();
};
std::vector<Button> buttons;

/*
 * SUBROUTINES
 */
void initialize();
void setMouseBlockSize(float size);
void pushSegment(const sfu::Line& segment);
void popSegment();
void pushBlock(const sf::Vector2f& pos);
void popBlock();
void setBrush(Brush b);
void castAllLights();
void click();
void updateOnMouseMove();
void updateOnMouseScroll(int d);
/*
 * MAIN
 */
int main(){
    
    initialize();
    
    return 0;
}

void initialize(){
    w.setFramerateLimit(60);
    lighting.adjustFog(w.getView());
    int colors = sizeof(BG_COLORS) / sizeof(*BG_COLORS);
    for (int i = 0; i < COLS * ROWS; i++){
        int p1 = i*4;
        int p2 = p1+1;
        int p3 = p1+2;
        int p4 = p1+3;
        float x = CELL_W * (i % COLS);
        float y = CELL_H * (i / COLS);
        background[p1].color =
            background[p2].color = 
            background[p3].color = 
            background[p4].color = BG_COLORS[i % colors];
        background[p1].position = {x, y};
        background[p2].position = {x, y + CELL_H};
        background[p3].position = {x + CELL_W, y + CELL_H};
        background[p4].position = {x + CELL_W, y};
    }
    setMouseBlockSize(CELL_W);
    brush = NONE;
    lineStarted = false;
    control = false;
    shift = false;
    alt = false;
}
void setMouseBlockSize(float size){
    mouseBlock[7].position = 
        mouseBlock[0].position = {-size/2, -size/2};
    mouseBlock[1].position = 
        mouseBlock[2].position = {size/2, -size/2};
    mouseBlock[3].position = 
        mouseBlock[4].position = {size/2, size/2};
    mouseBlock[5].position = 
        mouseBlock[6].position = {-size/2, size/2};
    blockSize = size;
    if(brush == BLOCK){
        popBlock();
        pushBlock(sf::Vector2f(sf::Mouse::getPosition(w)));
    }
}
void pushSegment(const sfu::Line& segment){
    lighting.m_segmentPool.push_back(segment);
    segmentVertices.append(sf::Vertex(segment.m_origin));
    segmentVertices.append(sf::Vertex(segment.point(1.f)));
}
void popSegment(){
    lighting.m_segmentPool.pop_back();
    segmentVertices.resize(segmentVertices.getVertexCount() - 2);
}
void pushBlock(const sf::Vector2f& pos){
    const sf::Vector2f points[] = {
        pos + mouseBlock[0].position,
        pos + mouseBlock[2].position,
        pos + mouseBlock[4].position,
        pos + mouseBlock[6].position,
    };
    sfu::Polygon p(points);
    for(auto& l: p.lines){
        pushSegment(l);
    }
}
void popBlock(){
    for(int i = 0; i < 4; i++){
        popSegment();
    }
}
void drawBrush(){
    sf::Transform t;
    t.translate(sf::Vector2f(sf::Mouse::getPosition(w)));
    switch(brush){
    case LINE:
        w.draw(sf::CircleShape(1.5f), sf::RenderStates(t));
        break;
    case BLOCK:
        w.draw(mouseBlock, sf::RenderStates(t));
        break;
    default:
        break;
    }
}
void setBrush(Brush b){
    if(b != brush){
        if(b == RADIAL){
            lighting.addLightSource(&radialLight);
        }else{
            lighting.removeLightSource(&radialLight);
        }
        if(b == DIRECTED){
            lighting.addLightSource(&directedLight);
        }else{
            lighting.removeLightSource(&directedLight);
        }
        if(b == BLOCK){
            pushBlock(sf::Vector2f(sf::Mouse::getPosition(w)));
        }
        if(brush == BLOCK){
            popBlock();
        }
        if(lineStarted){
            popSegment();
            lineStarted = false;
        }
        brush = b;
        updateOnMouseMove();
    }
}
void castAllLights(){
    for(auto& l: lights){
        l -> castLight();
    }
}
void click(){
    sf::Vector2f mp(sf::Mouse::getPosition(w));
    for(auto& button: buttons){
        if(button.getGlobalBounds().contains(mp)){
            button.function();
            return;
        }
    }
    switch(brush){
    case RADIAL:
        lights.emplace_back(new candle::RadialLight(radialLight));
        break;
    case DIRECTED:
        lights.emplace_back(new candle::DirectedLight(directedLight));
        break;
    case LINE:
        if(!lineStarted){
            pushSegment(sfu::Line(mp, 0.f));
        }
        lineStarted = !lineStarted;
        break;
    case BLOCK:
        pushBlock(mp);
        break;
    default:
        break;
    }
}
void updateOnMouseMove(){
    sf::Vector2f mp(sf::Mouse::getPosition(w));
    switch(brush){
    case BLOCK:
        popBlock();
        pushBlock(mp);
        castAllLights();
        break;
    case RADIAL:
        radialLight.setPosition(mp);
        radialLight.castLight();
        break;
    case DIRECTED:
        directedLight.setPosition(mp);
        directedLight.castLight();
        break;
    case LINE:
        if(lineStarted){
            int n = lighting.m_segmentPool.size();
            sf::Vector2f orig = lighting.m_segmentPool[n-1].m_origin;
            popSegment();
            pushSegment(sfu::Line(orig, mp));
            castAllLights();
        }
    default:
        break;
    }
}
void updateOnMouseScroll(int d){
    switch(brush){
    case RADIAL:
        if(control){
            radialLight.rotate(6 * d);
        }else if(shift){
            radialLight.setBeamAngle(radialLight.getBeamAngle() + d*5);
        }else{
            radialLight.setRange(radialLight.getRange() + d*5);
        }
        radialLight.castLight();
        break;
    case DIRECTED:
        if(control){
            directedLight.rotate(6 * d);
        }else if(shift){
            directedLight.setBeamWidth(directedLight.getBeamWidth() + d*5);
        }else if(alt){
            directedLight.setBeamInclination(directedLight.getBeamInclination() + d*5);
        }else{
            directedLight.setRange(directedLight.getRange() + d*5);
        }
        directedLight.castLight();
        break;
    case BLOCK:
        setMouseBlockSize(blockSize + d*CELL_W);
        break;
    default:
        break;
    }
}
void clearLights(){
    lights.clear();
    lighting.clear();
    if(brush == RADIAL){
        lighting.addLightSource(&radialLight);
    }else if(brush == DIRECTED){
        lighting.addLightSource(&directedLight);
    }
}
void clearSegments(){
    segmentVertices.clear();
    lighting.m_segmentPool.clear();
}
void clearAll(){
    clearLights();
    clearSegments();
}
