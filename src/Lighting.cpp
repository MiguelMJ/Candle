#include "Candle/Lighting.hpp"

namespace candle{

    sf::BlendMode l_lightBlend(
        sf::BlendMode::Factor::Zero,              // color src
        sf::BlendMode::Factor::One,               // color dst
        sf::BlendMode::Equation::Add,             // color eq
        sf::BlendMode::Factor::Zero,              // alpha src
        sf::BlendMode::Factor::OneMinusSrcAlpha,  // alpha dst
        sf::BlendMode::Equation::Add);            // alpha eq
    sf::BlendMode l_glowBlend = sf::BlendAdd;
    
    Lighting::Lighting()
    : m_fogQuad(sf::Quads, 4)
    , m_fogColor(sf::Color::Black)
    {
        setFogSize(10,10);
    }
    Lighting::Lighting(const Lighting& ll)
    : m_lights(ll.m_lights)
    , m_fogQuad(ll.m_fogQuad)
    , m_fogOffset(ll.m_fogOffset)
    , m_fogColor(ll.m_fogColor)
    , m_segmentPool(ll.m_segmentPool)
    {
        setFogSize(ll.getFogSize());
        setFogPosition(ll.getFogPosition());
        updateFog();
    }
    void  Lighting::draw(sf::RenderTarget& t, sf::RenderStates s) const{
        s.texture = &m_fogTexture.getTexture();
        t.draw(m_fogQuad, s);
        s.blendMode = l_glowBlend;
        for(auto& ls: m_lights){
            if(ls->m_glow){
                t.draw(*ls, s);
            }
        }
    }
    void Lighting::addLightSource(LightSource* ls) {
        ls->m_ptrSegmentPool.insert(&m_segmentPool);
        ls->m_ptrSegmentPool.insert(&m_boundsSegments);
        m_lights.insert(ls);
    }
    void Lighting::removeLightSource(LightSource* ls){
        ls->m_ptrSegmentPool.erase(&m_segmentPool);
        ls->m_ptrSegmentPool.erase(&m_boundsSegments);
        m_lights.erase(ls);
    }
    void Lighting::clear(){
        m_lights.clear();
    }
    void Lighting::setFogSize(float x, float y){
        m_fogTexture.create(x,y);
        m_fogTexture.setSmooth(true);
        m_fogQuad[0].position =
        m_fogQuad[0].texCoords = {0,0};
        m_fogQuad[1].position =
        m_fogQuad[1].texCoords = {x,0};
        m_fogQuad[2].position =
        m_fogQuad[2].texCoords = {x,y};
        m_fogQuad[3].position =
        m_fogQuad[3].texCoords = {0,y};
        candle::move(m_fogQuad, m_fogOffset);
        m_boundsSegments.clear();
        sf::FloatRect boundRec(m_fogQuad[0].position,m_fogQuad[2].position);
        auto segs = candle::segments(boundRec);
        m_boundsSegments.insert(m_boundsSegments.begin(),segs.begin(), segs.end());
    }
    void Lighting::setFogSize(sf::Vector2f size){
        setFogSize(size.x, size.y);
    }
    sf::Vector2f Lighting::getFogSize() const{
        return m_fogQuad[2].position - m_fogQuad[0].position;
    }
    void Lighting::setFogPosition(float x, float y){
        setFogPosition(sf::Vector2f(x, y));
    }
    void Lighting::setFogPosition(sf::Vector2f position){
        sf::Vector2f delta = position - m_fogOffset;
        candle::move(m_fogQuad, delta);
        m_fogOffset += delta;
        for(auto& seg : m_boundsSegments){
            seg.first += delta;
            seg.second += delta;
        }
    }
    sf::Vector2f Lighting::getFogPosition() const{
        return m_fogOffset;
    }
    void Lighting::adjustFog(const sf::View& view){
        auto size = view.getSize();
        auto center = view.getCenter();
        auto position = center - size/2.0f;
        setFogPosition(position.x, position.y);
        setFogSize(size.x, size.y);
    }
    void Lighting::setFogColor(sf::Color c){
        m_fogColor.r = c.r;
        m_fogColor.g = c.g;
        m_fogColor.b = c.b;
    }
    sf::Color Lighting::getFogColor() const{
        auto color = m_fogColor;
        color.a = 255;
        return color;
    }
    void Lighting::setFogOpacity(float o){
        m_fogColor.a = 255 * o;
    }
    float Lighting::getFogOpacity() const{
        return (float)m_fogColor.a/255;
    }
    void Lighting::updateFog(){
        if(m_fogColor.a > 0){
            m_fogTexture.clear(m_fogColor);
            sf::RenderStates fogrs;
            fogrs.blendMode = l_lightBlend;
            fogrs.transform.translate(-m_fogOffset);
            for(auto& ls: m_lights){
                m_fogTexture.draw(*ls, fogrs);
            }
            m_fogTexture.display();
        }
    }
}
