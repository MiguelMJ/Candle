#include <iostream>
#include <iomanip>
#include <memory>
#include <ctime>
#include <cmath>

#include "sfml-util/geometry/Polygon.hpp"
#include "sfml-util/graphics/VertexArray.hpp"

#include "Candle/LightingArea.hpp"
#include "Candle/LightSource.hpp"
#include "Candle/RadialLight.hpp"
#include "Candle/DirectedLight.hpp"

/*
 * AUXILIAR
 */
float clamp(float x){
    return std::max(0.f, std::min(1.f, x));
}
struct App{
    /*
    * WINDOW
    */
    constexpr static const float WIDTH = 700.f;
    constexpr static const float HEIGHT = 700.f;
    constexpr static const float MENU_W = HEIGHT/8.f;
    sf::RenderWindow w;
    sf::View sandboxView, menuView;

    /*
    * LIGHTING
    */
    candle::LightingArea lighting;
    bool glow = true;
    std::vector<std::shared_ptr<candle::LightSource>> lights1; // all
    std::vector<std::shared_ptr<candle::LightSource>> lights2; // glowing
    sf::VertexArray edgeVertices;

    /*
    * BACKGROUND
    */
    static const int ROWS = 16;
    static const int COLS = 16;
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
    struct Button: sf::Drawable, sf::Transformable{
        static sf::Color buttonA1;
        static sf::Color buttonA2;
        static sf::Color buttonZ1;
        static sf::Color buttonZ2;
        static int BC;
        sf::RectangleShape rect;
        sf::Drawable* icon;
        void (*function)(App*);
        Button(sf::Drawable* d, void (*f)(App*)){
            float bw = (MENU_W-4);
            rect.setFillColor(buttonZ1);
            rect.setOutlineColor(buttonZ2);
            rect.setOutlineThickness(2);
            rect.setSize({bw, bw});
            setPosition(2, 2 + (BC)*(bw + 4));
            icon = d;
            function = f;
            BC++;
        }
        ~Button(){
            // delete icon;
        }
        bool contains(const sf::Vector2f& p) const{
            return getTransform().transformRect(rect.getGlobalBounds()).contains(p);
        }
        void draw(sf::RenderTarget& t, sf::RenderStates st) const{
            st.transform = this->getTransform();
            t.draw(rect, st);
            if(icon != NULL)
                t.draw(*icon, st);
        }
    };
    std::vector<Button> buttons;

    /*
    * SUBROUTINES
    */
    sf::Vector2f getMousePosition(){
        sf::Vector2f mp = w.mapPixelToCoords(sf::Mouse::getPosition(w));
        if(control){
            mp = {
                CELL_W * (0.5f + std::round(mp.x/CELL_W - 0.5f)),
                CELL_H * (0.5f + std::round(mp.y/CELL_H - 0.5f))
            };
        }
        return mp;
    }
    App()
    : lighting(candle::LightingArea::FOG, {0.f,0.f}, {WIDTH, HEIGHT})
    {
        w.create(sf::VideoMode(WIDTH+MENU_W, HEIGHT), "Candle - demo");
        w.setFramerateLimit(60);
        float totalWidth = WIDTH + MENU_W;
        edgeVertices.setPrimitiveType(sf::Lines);
        background.setPrimitiveType(sf::Quads);
        background.resize(ROWS * COLS * 4);
        mouseBlock.setPrimitiveType(sf::Lines);
        mouseBlock.resize(8);
        sandboxView.setSize(WIDTH, HEIGHT);
        sandboxView.setCenter(WIDTH/2.f, HEIGHT/2.f);
        sandboxView.setViewport({0.f, 0.f, WIDTH/totalWidth, 1.f});
        menuView.setSize(MENU_W, HEIGHT);
        menuView.setCenter(MENU_W/2, HEIGHT/2);
        menuView.setViewport({WIDTH/totalWidth, 0.f, MENU_W/totalWidth, 1.f});
        radialLight.setRange(100.f);
        directedLight.setRange(200.f);
        directedLight.setBeamWidth(200.f);
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
        
        auto i1 = new sf::CircleShape(MENU_W/3, 60);
        i1->setFillColor({255,255,150,255});
        i1->setOutlineColor(sf::Color::White);
        i1->setOutlineThickness(2);
        i1->move(MENU_W/6, MENU_W/6);
        buttons.emplace_back(i1, [](App* app){ app->setBrush(RADIAL); });
        
        auto i2 = new sf::RectangleShape({MENU_W*2/3, MENU_W/2});
        i2->move(MENU_W/6, MENU_W/4);
        i2->setFillColor({255,255,150,255});
        i2->setOutlineColor(sf::Color::White);
        i2->setOutlineThickness(1);
        buttons.emplace_back(i2, [](App* app){ app->setBrush(DIRECTED); });
        
        auto i3 = new sf::RectangleShape({MENU_W/2, MENU_W/2});
        i3->move(MENU_W/4, MENU_W/4);
        i3->setFillColor({25,25,25,255});
        buttons.emplace_back(i3, [](App* app){ app->setBrush(BLOCK); });
        
        auto i4 = new sf::RectangleShape({MENU_W, 3});
        i4->move(MENU_W*2/15, MENU_W*2/15);
        i4->rotate(45.f);
        i4->setFillColor({25,25,25,255});
        buttons.emplace_back(i4, [](App* app){ app->setBrush(LINE); });
        
        auto  i7 = new sf::VertexArray(sf::Quads, 12);
        for(int i=0; i < 12; i++){
            (*i7)[i] = background[i];
        }
        sfu::transform(*i7, {MENU_W*2/7/CELL_W, 0, MENU_W/14 - 1,
                            0, MENU_W*2/7/CELL_H, MENU_W*5/14 - 1,
                            0, 0, 1
                            });
        sfu::darken(*i7, 0.2);
        buttons.emplace_back(i7, [](App *app){ app->lighting.setAreaOpacity(clamp(app->lighting.getAreaOpacity() - 0.1)); });
        
        auto  i8 = new sf::VertexArray(*i7);
        sfu::darken(*i8, 0.5);
        buttons.emplace_back(i8, [](App *app){ app->lighting.setAreaOpacity(clamp(app->lighting.getAreaOpacity() + 0.1)); });
        
        auto i5 = new sf::VertexArray(sf::Quads, 8);
        (*i5)[0].position = {1, 0};
        (*i5)[1].position = {2, 0};
        (*i5)[2].position = {2, 3};
        (*i5)[3].position = {1, 3};
        (*i5)[4].position = {0, 1};
        (*i5)[5].position = {3, 1};
        (*i5)[6].position = {3, 2};
        (*i5)[7].position = {0, 2};
        sfu::setColor(*i5, {255,255,150,255});
        float a = M_PI/4;
        float s = MENU_W*2/9;
        sfu::transform(*i5, {s*std::cos(a), s*-std::sin(a), MENU_W/2,
                            s*std::sin(a), s*std::cos(a), 0,
                            0, 0, 1
                            });
        buttons.emplace_back(i5, [](App* app){ app->clearLights(); });
        
        auto i6 = new sf::VertexArray(*i5);
        sfu::setColor(*i6, {25,25,25,255});
        buttons.emplace_back(i6, [](App *app){ app->clearEdges(); app->castAllLights(); });
        
        
        
    }
    void capture(){
        sf::Texture tex;
        tex.create(w.getSize().x, w.getSize().y);
        tex.update(w);
        
        sf::RenderTexture rt;
        rt.create(WIDTH, HEIGHT);
        rt.setView(sf::View({WIDTH/2, HEIGHT/2}, {WIDTH, HEIGHT}));
        rt.draw(sf::Sprite(tex));
        rt.display();
        
        std::string name = "candle-capture-";
        char timestr[13];
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timestr,13,"%y%m%d%H%M%S",timeinfo);
        name += timestr;
        name += ".png";
        
        if(!rt.getTexture().copyToImage().saveToFile(name)){
            exit(1);
        }
    }
    void setMouseBlockSize(float size){
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
    void pushEdge(const sfu::Line& edge){
        lighting.m_edgePool.push_back(edge);
        edgeVertices.append(sf::Vertex(edge.m_origin));
        edgeVertices.append(sf::Vertex(edge.point(1.f)));
    }
    void popEdge(){
        lighting.m_edgePool.pop_back();
        edgeVertices.resize(edgeVertices.getVertexCount() - 2);
    }
    void pushBlock(const sf::Vector2f& pos){
        const sf::Vector2f points[] = {
            pos + mouseBlock[0].position,
            pos + mouseBlock[2].position,
            pos + mouseBlock[4].position,
            pos + mouseBlock[6].position,
        };
        sfu::Polygon p(points, 4);
        for(auto& l: p.lines){
            pushEdge(l);
        }
    }
    void popBlock(){
        for(int i = 0; i < 4; i++){
            popEdge();
        }
    }
    void drawBrush(){
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
    void setBrush(Brush b){
        if(b != brush){
            if(b == BLOCK){
                pushBlock(getMousePosition());
            }
            if(brush == BLOCK){
                popBlock();
                castAllLights();
            }
            if(lineStarted){
                popEdge();
                castAllLights();
                lineStarted = false;
            }
            brush = b;
            updateOnMouseMove();
        }
    }
    void castAllLights(){
        for(auto& l: lights1){
            l -> castLight();
        }
    }
    void click(){
        sf::Vector2f mp = getMousePosition();
        if(mp.x > WIDTH){
            mp.x -= WIDTH;
            bool press=false;
            for(auto& button: buttons){
                if(button.contains(mp)){
                    button.rect.setFillColor(sf::Color(Button::buttonA1));
                    button.rect.setOutlineColor(sf::Color(Button::buttonA2));
                    button.function(this);
                    press = true;
                }else{
                    button.rect.setFillColor(sf::Color(Button::buttonZ1));
                    button.rect.setOutlineColor(sf::Color(Button::buttonZ2));
                }
            }
            if(!press) setBrush(NONE);
        }else{
            switch(brush){
            case RADIAL:{
                std::shared_ptr<candle::LightSource> nl(new candle::RadialLight(radialLight));
                lights1.push_back(nl);
                if(glow){
                    lights2.push_back(nl);
                }
            }
            break;
            case DIRECTED:{
                std::shared_ptr<candle::LightSource> nl(new candle::DirectedLight(directedLight));
                lights1.push_back(nl);
                if(glow){
                    lights2.push_back(nl);
                }
            }
            break;
            case LINE:
                pushEdge(sfu::Line(mp, 0.f));
                lineStarted = true;
                break;
            case BLOCK:
                pushBlock(mp);
                break;
            default:
                break;
            }
        }
    }
    void updateOnMouseMove(){
        sf::Vector2f mp = getMousePosition();
        if(mp.x < WIDTH){
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
                    int n = lighting.m_edgePool.size();
                    sf::Vector2f orig = lighting.m_edgePool[n-1].m_origin;
                    popEdge();
                    pushEdge(sfu::Line(orig, mp));
                    castAllLights();
                }
            default:
                break;
            }
        }
    }
    void updateOnMouseScroll(int d){
        if(getMousePosition().x < WIDTH){
            switch(brush){
            case RADIAL:
                if(alt){
                    radialLight.rotate(d*6);
                }else if(shift){
                    radialLight.setBeamAngle(radialLight.getBeamAngle() + d*5);
                }else{
                    radialLight.setRange(std::max(0.f, radialLight.getRange() + d*10));
                }
                radialLight.castLight();
                break;
            case DIRECTED:
                if(alt){
                    directedLight.rotate(6 * d);
                }else if(shift){
                    directedLight.setBeamWidth(directedLight.getBeamWidth() + d*5);
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
    }
    void updateOnPressKey(sf::Keyboard::Key k){
        switch(k){
        case sf::Keyboard::P:
            capture();
            break;
        case sf::Keyboard::Q:
        case sf::Keyboard::Escape:
            w.close();
            break;
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
            lighting.setAreaOpacity(clamp(lighting.getAreaOpacity()+0.1));
            break;
        case sf::Keyboard::Z:
            lighting.setAreaOpacity(clamp(lighting.getAreaOpacity()-0.1));
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
                glow = !glow;
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
            if(alt){
                clearEdges();
            }else if(shift){
                clearLights();
            }else{
                clearAll();
            }
            castAllLights();
            break;
        default:
            break;
        }
    }
    void updateOnReleaseKey(sf::Keyboard::Key k){
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
    void clearLights(){
        lights1.clear();
        lights2.clear();
    }
    void clearEdges(){
        edgeVertices.clear();
        lineStarted = false;
        if(brush == BLOCK) pushBlock(getMousePosition());
    }
    void clearAll(){
        clearLights();
        clearEdges();
    }
    void mainLoop(){
        //candle::initializeTextures();
        sf::Clock clock;
        while(w.isOpen()){
            sf::Event e;
            while(w.pollEvent(e)){
                switch(e.type){
                case sf::Event::Closed:
                    w.close();
                    break;
                case sf::Event::MouseMoved:
                    updateOnMouseMove();
                    break;
                case sf::Event::MouseWheelScrolled:
                    updateOnMouseScroll((0 < e.mouseWheelScroll.delta) - (e.mouseWheelScroll.delta < 0));
                    break;
                case sf::Event::KeyPressed:
                    updateOnPressKey(e.key.code);
                    break;
                case sf::Event::KeyReleased:
                    updateOnReleaseKey(e.key.code);
                    break;
                case sf::Event::MouseButtonPressed:
                    if(e.mouseButton.button == sf::Mouse::Left){
                        click();
                    }else{
                        setBrush(NONE);
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if(e.mouseButton.button == sf::Mouse::Left){
                        lineStarted = false;
                        for(auto& b: buttons){
                            b.rect.setFillColor(Button::buttonZ1);
                            b.rect.setOutlineColor(Button::buttonZ2);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            
            lighting.clear();
            for(auto& l: lights1){
                lighting.draw(*l);
            }
            if(brush == RADIAL){
                lighting.draw(radialLight);
            }else if(brush == DIRECTED){
                lighting.draw(directedLight);
            }
            lighting.display();
            
            
            w.clear();
            
            w.setView(menuView);
            for(auto& b: buttons){
                w.draw(b);
            }
            
            w.setView(sandboxView);
            w.draw(background);
            w.draw(lighting);
            for(auto& l: lights2){
                w.draw(*l);
            }
            if(glow){
                if(brush == RADIAL){
                    w.draw(radialLight);
                }else if(brush == DIRECTED){
                    w.draw(directedLight);
                }
            }
            w.draw(edgeVertices);
            drawBrush();
            
            w.display();
            
            sf::Time dt = clock.restart();
            int fps = int(std::round(1.f/dt.asSeconds()));
            w.setTitle("Candle demo [" 
                        + std::to_string(fps) 
                        + " fps: " 
                        + std::to_string(dt.asMilliseconds()) 
                        + " ms] ("
                        + std::to_string(lights1.size() + (brush==RADIAL || brush==DIRECTED))
                        + " Light/s  "
                        + std::to_string(lighting.m_edgePool.size())
                        + " Edge/s)");
        }
    }
};
int App::Button::BC = 0;
sf::Color App::Button::buttonA1({50,50,250,255});
sf::Color App::Button::buttonA2({40,40,40,255});
sf::Color App::Button::buttonZ1({50,50,50,255});
sf::Color App::Button::buttonZ2({20,20,20,255});
/*
 * MAIN
 */
int main(){
    App app;
    app.mainLoop();
    return 0;
}
