#include "Candle/LightingArea.hpp"
#include "Candle/graphics/VertexArray.hpp"

#include <iostream>

namespace candle{
    
    sf::BlendMode l_substractAlpha(
        sf::BlendMode::Factor::Zero,              // color src
        sf::BlendMode::Factor::One,               // color dst
        sf::BlendMode::Equation::Add,             // color eq
        sf::BlendMode::Factor::Zero,              // alpha src
        sf::BlendMode::Factor::OneMinusSrcAlpha,  // alpha dst
        sf::BlendMode::Equation::Add);            // alpha eq
    
    void LightingArea::initializeRenderTexture(const sf::Vector2f& size){
        try { 
            if (!m_renderTexture.resize({ uint32_t(size.x), uint32_t(size.y) }))
                throw std::runtime_error("Failed to resize LightingArea RenderTexture!");

        }
        catch (const std::exception& e) {
            std::cerr << "Render texture resize error: " << e.what() << std::endl;
        }
        m_renderTexture.setSmooth(true);

        m_baseTextureTriangles[0].position =
        m_areaTriangles[0].position =
        m_areaTriangles[0].texCoords = {0, 0};
        m_baseTextureTriangles[1].position =
        m_areaTriangles[1].position =
        m_areaTriangles[1].texCoords = {size.x, 0};
        m_baseTextureTriangles[2].position =
        m_areaTriangles[2].position =
        m_areaTriangles[2].texCoords = {size.x, size.y};

        m_baseTextureTriangles[3].position =
        m_areaTriangles[3].position =
        m_areaTriangles[3].texCoords = { 0, 0 };
        m_baseTextureTriangles[4].position =
        m_areaTriangles[4].position =
        m_areaTriangles[4].texCoords = { size.x, size.y };
        m_baseTextureTriangles[5].position =
        m_areaTriangles[5].position =
        m_areaTriangles[5].texCoords = { 0, float(size.y) };

    }
    
    LightingArea::LightingArea(Mode mode, const sf::Vector2f& position, const sf::Vector2f& size)
    : m_baseTextureTriangles(sf::PrimitiveType::Triangles, 6)
    , m_areaTriangles(sf::PrimitiveType::Triangles, 6)
    , m_color(sf::Color::White)
    {
        m_opacity = 1;
        m_mode = mode;
        m_baseTexture = nullptr;
        initializeRenderTexture(size);
        Transformable::setPosition(position);
    }
    
    LightingArea::LightingArea(Mode mode, const sf::Texture* t, sf::FloatRect r)
    : m_baseTextureTriangles(sf::PrimitiveType::Triangles, 6)
    , m_areaTriangles(sf::PrimitiveType::Triangles, 6)
    , m_color(sf::Color::White)
    {
        m_opacity = 1;
        m_mode = mode;
        setAreaTexture(t, r);
    }
    
    sf::FloatRect LightingArea::getLocalBounds () const{
        return m_areaTriangles.getBounds();
    }
    
    sf::FloatRect LightingArea::getGlobalBounds () const{
        return Transformable::getTransform().transformRect(m_areaTriangles.getBounds());
    }
    
    void  LightingArea::draw(sf::RenderTarget& t, sf::RenderStates s) const{
        if(m_opacity > 0.f){
            if(m_mode == AMBIENT){
                s.blendMode = sf::BlendAdd;
            }
            s.transform *= Transformable::getTransform();
            s.texture = &m_renderTexture.getTexture();
            t.draw(m_areaTriangles, s);
        }
    }
    
    void LightingArea::clear(){
        if(m_baseTexture != nullptr){
            m_renderTexture.clear(sf::Color::Transparent);
            m_renderTexture.draw(m_baseTextureTriangles, m_baseTexture);
        }else{
            m_renderTexture.clear(getActualColor());
        }
    }
    
    void LightingArea::setAreaColor(sf::Color c){
        m_color = c;
        sfu::setColor(m_baseTextureTriangles, getActualColor());
    }
    
    sf::Color LightingArea::getAreaColor() const{
        return m_color;
    }
    
    sf::Color LightingArea::getActualColor() const{
        sf::Color ret(m_color);
        ret.a *= m_opacity;
        return ret;
    }
    
    void LightingArea::setAreaOpacity(uint8_t o){
        m_opacity = o;
        sfu::setColor(m_baseTextureTriangles, getActualColor());
    }
    
    float LightingArea::getAreaOpacity() const{
    return m_opacity;
    }
    
    void LightingArea::draw(const LightSource& light){
        if(m_opacity > 0.f && m_mode == FOG){
            sf::RenderStates fogrs;
            fogrs.blendMode = l_substractAlpha;
            fogrs.transform *= Transformable::getTransform().getInverse();
            m_renderTexture.draw(light, fogrs);
        }
    }
    
    void LightingArea::setAreaTexture(const sf::Texture* texture, sf::FloatRect rect){
        m_baseTexture = texture;
        if(rect.size.x == 0 && rect.size.y == 0 && texture != nullptr){
            rect.size.x = float(texture->getSize().x);
            rect.size.y = float(texture->getSize().y);
        }
        initializeRenderTexture(rect.size);
        setTextureRect(rect);
    }
    
    const sf::Texture* LightingArea::getAreaTexture() const{
        return m_baseTexture;
    }
    
    void LightingArea::setTextureRect(const sf::FloatRect& rect){
        m_baseTextureTriangles[0].texCoords = sf::Vector2f(rect.position.x, rect.position.y);
        m_baseTextureTriangles[1].texCoords = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y);
        m_baseTextureTriangles[2].texCoords = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
        m_baseTextureTriangles[3].texCoords = sf::Vector2f(rect.position.x, rect.position.y);
        m_baseTextureTriangles[4].texCoords = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
        m_baseTextureTriangles[5].texCoords = sf::Vector2f(rect.position.x, rect.position.y + rect.size.y);
    }
    
    sf::IntRect LightingArea::getTextureRect() const{
        return m_baseTextureRect;
    }
    
    void LightingArea::setMode(Mode mode){
        m_mode = mode;
    }
    
    LightingArea::Mode LightingArea::getMode() const{
        return m_mode;
    }
    
    void LightingArea::display(){
        m_renderTexture.display();
    }
}
