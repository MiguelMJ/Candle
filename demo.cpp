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

struct context{
/*
 * WINDOW
 */
const float WIDTH = 700;
const float HEIGHT = 700;
const float MENU_W = 200;
sf::RenderWindow w;
sf::View sandboxView, menuView;

/*
 * LIGHTING
 */
candle::Lighting lighting;
std::vector<std::unique_ptr<candle::LightSource>> lights;
sf::VertexArray segmentVertices;

/*
 * BACKGROUND
 */
const int ROWS = 16;
const int COLS = 16;
float CELL_W = WIDTH / COLS;
float CELL_H = HEIGHT / ROWS;
sf::VertexArray background;

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

sf::VertexArray mouseBlock;
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
sf::Vector2f getMousePosition();
void initialize();
void setMouseBlockSize(float size);
void pushSegment(const sfu::Line& segment);
void popSegment();
void pushBlock(const sf::Vector2f& pos);
void popBlock();
void drawBrush();
void setBrush(Brush b);
void castAllLights();
void click();
void updateOnMouseMove();
void updateOnMouseScroll(int d);
void updateOnPressKey(sf::Keyboard::Key k);
void updateOnReleaseKey(sf::Keyboard::Key k);
void clearLights();
void clearSegments();
void clearAll();
};
/*
 * MAIN
 */
int main(){
    context c;
    candle::initializeTextures();
    c.initialize();
    
    while(c.w.isOpen()){
        sf::Event e;
        while(c.w.pollEvent(e)){
            switch(e.type){
            case sf::Event::Closed:
                c.w.close();
                break;
            case sf::Event::MouseMoved:
                c.updateOnMouseMove();
                break;
            case sf::Event::MouseWheelScrolled:
                c.updateOnMouseScroll((0 < e.mouseWheelScroll.delta) - (e.mouseWheelScroll.delta < 0));
                break;
            case sf::Event::KeyPressed:
                c.updateOnPressKey(e.key.code);
                break;
            case sf::Event::KeyReleased:
                c.updateOnReleaseKey(e.key.code);
                break;
            case sf::Event::MouseButtonPressed:
                if(e.mouseButton.button == sf::Mouse::Left){
                    c.click();
                }
                break;
            case sf::Event::MouseButtonReleased:
                if(e.mouseButton.button == sf::Mouse::Left){
                    c.lineStarted = false;
                }
                break;
            default:
                break;
            }
        }
        
        c.lighting.updateFog();
        
        c.w.clear();
        
        c.w.setView(c.menuView);
        
        c.w.setView(c.sandboxView);
        c.w.draw(c.background);
        c.w.draw(c.lighting);
        c.w.draw(c.segmentVertices);
        c.drawBrush();
        
        c.w.display();
    }
    
    return 0;
}

/*
 * AUXILIAR
 */
float clamp(float x){
    return std::max(0.f, std::min(1.f, x));
}
/*
 * SUBROUTINES - Definition
 */
sf::Vector2f context::getMousePosition(){
    return w.mapPixelToCoords(sf::Mouse::getPosition(w));
}
void context::initialize(){
    w.create(sf::VideoMode(WIDTH+MENU_W, HEIGHT), "Candle - demo");
    w.setFramerateLimit(60);
    float totalWidth = WIDTH + MENU_W;
    segmentVertices.setPrimitiveType(sf::Lines);
    background.setPrimitiveType(sf::Quads);
    background.resize(ROWS * COLS * 4);
    mouseBlock.setPrimitiveType(sf::Lines);
    mouseBlock.resize(8);
    sandboxView.setSize(WIDTH, HEIGHT);
    sandboxView.setCenter(WIDTH/2.f, HEIGHT/2.f);
    sandboxView.setViewport({0.f, 0.f, WIDTH/totalWidth, 1.f});
    menuView.setSize(MENU_W, HEIGHT);
    menuView.setViewport({WIDTH/totalWidth, 0.f, MENU_W/totalWidth, 1.f});
    lighting.adjustFog(sandboxView);
    radialLight.setRange(100.f);
    directedLight.setRange(100.f);
    directedLight.setBeamWidth(100.f);
    static const sf::Color BG_COLORS[] = {
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Red
    };
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
void context::setMouseBlockSize(float size){
    if(size > 0){
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
            pushBlock(getMousePosition());
            castAllLights();
        }
    }
}
void context::pushSegment(const sfu::Line& segment){
    lighting.m_segmentPool.push_back(segment);
    segmentVertices.append(sf::Vertex(segment.m_origin));
    segmentVertices.append(sf::Vertex(segment.point(1.f)));
}
void context::popSegment(){
    lighting.m_segmentPool.pop_back();
    segmentVertices.resize(segmentVertices.getVertexCount() - 2);
}
void context::pushBlock(const sf::Vector2f& pos){
    const sf::Vector2f points[] = {
        pos + mouseBlock[0].position,
        pos + mouseBlock[2].position,
        pos + mouseBlock[4].position,
        pos + mouseBlock[6].position,
    };
    sfu::Polygon p(points, 4);
    for(auto& l: p.lines){
        pushSegment(l);
    }
}
void context::popBlock(){
    for(int i = 0; i < 4; i++){
        popSegment();
    }
}
void context::drawBrush(){
    sf::Transform t;
    t.translate(getMousePosition());
    switch(brush){
    case LINE:
        w.draw(sf::CircleShape(1.5f), sf::RenderStates(t));
        break;
    default:
        break;
    }
}
void context::setBrush(Brush b){
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
            pushBlock(getMousePosition());
        }
        if(brush == BLOCK){
            popBlock();
            castAllLights();
        }
        if(lineStarted){
            popSegment();
            castAllLights();
            lineStarted = false;
        }
        brush = b;
        updateOnMouseMove();
    }
}
void context::castAllLights(){
    for(auto& l: lights){
        l -> castLight();
    }
}
void context::click(){
    sf::Vector2f mp = getMousePosition();
    for(auto& button: buttons){
        if(button.getGlobalBounds().contains(mp)){
            button.function();
            return;
        }
    }
    switch(brush){
    case RADIAL:
        lights.emplace_back(new candle::RadialLight(radialLight));
        lighting.addLightSource(lights.back().get());
        break;
    case DIRECTED:
        lights.emplace_back(new candle::DirectedLight(directedLight));
        lighting.addLightSource(lights.back().get());
        break;
    case LINE:
        pushSegment(sfu::Line(mp, 0.f));
        lineStarted = true;
        break;
    case BLOCK:
        pushBlock(mp);
        break;
    default:
        break;
    }
}
void context::updateOnMouseMove(){
    sf::Vector2f mp = getMousePosition();
    switch(brush){
    case BLOCK:
        if(control){
            mp = {
                CELL_W * (0.5f + std::round(mp.x/CELL_W - 0.5f)),
                CELL_H * (0.5f + std::round(mp.y/CELL_H - 0.5f))
            };
        }
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
void context::updateOnMouseScroll(int d){
    switch(brush){
    case RADIAL:
        if(control){
            radialLight.rotate(d*5);
        }else if(shift){
            radialLight.setBeamAngle(radialLight.getBeamAngle() + d*5);
        }else{
            radialLight.setRange(std::max(0.f, radialLight.getRange() + d*10));
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
            directedLight.setRange(std::max(0.f, directedLight.getRange() + d*10));
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
void context::updateOnPressKey(sf::Keyboard::Key k){
    switch(k){
    case sf::Keyboard::LControl:
        control = true;
        break;
    case sf::Keyboard::LAlt:
        alt = true;
        break;
    case sf::Keyboard::LShift:
        shift = true;
        break;
    case sf::Keyboard::R:
        setBrush(RADIAL);
        break;
    case sf::Keyboard::D:
        setBrush(DIRECTED);
        break;
    case sf::Keyboard::B:
        setBrush(BLOCK);
        break;
    case sf::Keyboard::L:
        setBrush(LINE);
        break;
    case sf::Keyboard::A:
        lighting.setFogOpacity(clamp(lighting.getFogOpacity()+0.1));
        break;
    case sf::Keyboard::Z:
        lighting.setFogOpacity(clamp(lighting.getFogOpacity()-0.1));
        break;
    case sf::Keyboard::S:
        if(brush == RADIAL || brush == DIRECTED){
            radialLight.setIntensity(clamp(radialLight.getIntensity()+0.1));
            directedLight.setIntensity(clamp(directedLight.getIntensity()+0.1));
        }
        break;
    case sf::Keyboard::X:
        if(brush == RADIAL || brush == DIRECTED){
            radialLight.setIntensity(clamp(radialLight.getIntensity()-0.1));
            directedLight.setIntensity(clamp(directedLight.getIntensity()-0.1));
        }
        break;
    case sf::Keyboard::G:
        if(brush == RADIAL || brush == DIRECTED){
            radialLight.setGlow(!radialLight.getGlow());
            directedLight.setGlow(!directedLight.getGlow());
        }
        break;
    case sf::Keyboard::F:
        if(brush == RADIAL || brush == DIRECTED){
            radialLight.setFade(!radialLight.getFade());
            directedLight.setFade(!directedLight.getFade());
        }
        break;
    case sf::Keyboard::C:
        if(brush == RADIAL || brush == DIRECTED){
            static const sf::Color L_COLORS[] = {
                sf::Color::White,
                sf::Color::Magenta,
                sf::Color::Cyan,
                sf::Color::Yellow
            };
            static int color_i = 0;
            int n = sizeof(L_COLORS)/sizeof(*L_COLORS);
            color_i = (color_i+1) % n;
            radialLight.setColor(L_COLORS[color_i]);
            directedLight.setColor(L_COLORS[color_i]);
        }
        break;
    case sf::Keyboard::Space:
        lineStarted = false;
        if(control){
            clearSegments();
            if(brush == BLOCK) pushBlock(getMousePosition());
        }else if(alt){
            clearLights();
        }else{
            clearAll();
            if(brush == BLOCK) pushBlock(getMousePosition());
        }
        castAllLights();
        break;
    default:
        break;
    }
}
void context::updateOnReleaseKey(sf::Keyboard::Key k){
    switch(k){
    case sf::Keyboard::LControl:
        control = false;
        break;
    case sf::Keyboard::LAlt:
        alt = false;
        break;
    case sf::Keyboard::LShift:
        shift = false;
        break;
    default:
        break;
    }
}
void context::clearLights(){
    lights.clear();
    lighting.clear();
    if(brush == RADIAL){
        lighting.addLightSource(&radialLight);
    }else if(brush == DIRECTED){
        lighting.addLightSource(&directedLight);
    }
}
void context::clearSegments(){
    segmentVertices.clear();
    lighting.m_segmentPool.clear();
}
void context::clearAll(){
    clearLights();
    clearSegments();
}
