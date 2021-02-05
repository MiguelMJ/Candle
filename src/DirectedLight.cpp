#include "Candle/DirectedLight.hpp"

#include <queue>

#include "sfml-util/geometry/Vector2.hpp"
#include "sfml-util/graphics/VertexArray.hpp"

namespace candle{
    void DirectedLight::draw(sf::RenderTarget& t, sf::RenderStates st) const{
        st.transform *= Transformable::getTransform();
        t.draw(m_polygon, st);
#ifdef CANDLE_DEBUG
        sf::RenderStates deb_s;
        deb_s.transform = st.transform;
        t.draw(m_debug, deb_s);
#endif
    }
    
    void DirectedLight::resetColor(){
        int quads = m_polygon.getVertexCount() / 4;
        for(int i = 0; i < quads; i++){
            float p1 = i*4;
            float p2 = p1+1;
            float p3 = p1+2;
            float p4 = p1+3;
            sf::Vector2f r1 = m_polygon[p1].position;
            sf::Vector2f r2 = m_polygon[p2].position;
            sf::Vector2f r3 = m_polygon[p4].position;
            sf::Vector2f r4 = m_polygon[p3].position;
            
            float dr1 = 1.f - m_fade * (sfu::magnitude(r2-r1) / m_range);
            float dr2 = 1.f - m_fade * (sfu::magnitude(r4-r3) / m_range);
            m_polygon[p1].color = m_polygon[p2].color =
                m_polygon[p3].color = m_polygon[p4].color = m_color;
            m_polygon[p2].color.a = m_color.a * dr1;
            m_polygon[p3].color.a = m_color.a * dr2;
        }
    }
    
    DirectedLight::DirectedLight(){
        m_polygon.setPrimitiveType(sf::Quads);
        m_polygon.resize(2);
        setBeamWidth(10.f);
        m_shouldRecast = true;
        // castLight();
    }
    
    void DirectedLight::setBeamWidth(float width){
        m_beamWidth = width;
        m_shouldRecast = true;
    }
    
    float DirectedLight::getBeamWidth() const{
        return m_beamWidth;
    }
    
    struct LineParam: public sfu::Line{
        float param;
        LineParam(float f, const sfu::Line& l)
            : sfu::Line(l)
            , param(f) { }
        LineParam(const sf::Vector2f& orig, const sf::Vector2f& dir, float p)
            : sfu::Line(orig, orig + dir)
            , param(p) { }
    };
    bool operator < (const LineParam& a, const LineParam& b){
        return a.param < b.param;
    }
    void DirectedLight::castLight(){
        sf::Transform trm = Transformable::getTransform();
        sf::Transform trm_i = trm.getInverse();
        
        float widthHalf = m_beamWidth/2.f;
        sf::FloatRect baseBeam(0, -widthHalf, m_range, m_beamWidth);
        
        sf::Vector2f lim1o = trm.transformPoint(0, -widthHalf);
        sf::Vector2f lim1d = trm.transformPoint(m_range, -widthHalf);
        sf::Vector2f lim2o = trm.transformPoint(0, widthHalf);
        sf::Vector2f lim2d = trm.transformPoint(m_range, widthHalf);
        
        float off = 0.01/sfu::magnitude(lim2o - lim1o);
        sf::Vector2f lightDir = lim1d - lim1o;
        
        sfu::Line lim1(lim1o, lim1d);
        sfu::Line lim2(lim2o, lim2d);
        sfu::Line raySrc(lim1o, lim2o);
        sfu::Line rayRng(lim1d, lim2d);
        
        std::priority_queue <LineParam> rays;
        
        rays.emplace(0.f, lim1);
        rays.emplace(1.f, lim2);
        for(auto& pool: m_ptrSegmentPool){
            for(auto& seg: *pool){
                float tRng, tSeg;
                if(
                    rayRng.intersection(seg, tRng, tSeg) == sfu::Line::SECANT
                    && tRng <= 1
                    && tRng >= 0
                    && tSeg <= 1
                    && tSeg >= 0
                ){
                    rays.emplace(raySrc.point(tRng), lightDir, tRng);
                }
                float t;
                sf::Vector2f end = seg.m_origin;
                if(baseBeam.contains(trm_i.transformPoint(end))){
                    raySrc.intersection(sfu::Line(end, end-lightDir), t);
                    rays.emplace(raySrc.point(t - off), lightDir, t - off);
                    rays.emplace(raySrc.point(t), lightDir, t);
                    rays.emplace(raySrc.point(t + off), lightDir, t + off);
                }
                end = seg.point(1.f);
                if(baseBeam.contains(trm_i.transformPoint(end))){
                    raySrc.intersection(sfu::Line(end, end-lightDir), t);
                    rays.emplace(raySrc.point(t - off), lightDir, t - off);
                    rays.emplace(raySrc.point(t), lightDir, t);
                    rays.emplace(raySrc.point(t + off), lightDir, t + off);
                }
            }
        }
        std::vector<sf::Vector2f> points;
        points.reserve(rays.size()*2);
#ifdef CANDLE_DEBUG
        int deb_r = rays.size()*2 + 4;
        m_debug.resize(deb_r);
        sfu::setColor(m_debug, sf::Color::Magenta);
        int i=0;
        m_debug[deb_r-1].color = m_debug[deb_r-2].color = sf::Color::Cyan;
        m_debug[deb_r-3].color = m_debug[deb_r-4].color = sf::Color::Yellow;
        m_debug[deb_r-1].position = {0, -widthHalf};
        m_debug[deb_r-2].position = {0, widthHalf};
        m_debug[deb_r-3].position = {m_range, -widthHalf};
        m_debug[deb_r-4].position = {m_range, widthHalf};
#endif
        while(!rays.empty()){
            LineParam r = rays.top();
        
            sf::Vector2f p1 = trm_i.transformPoint(r.m_origin);
            sf::Vector2f p2 = trm_i.transformPoint(castRay(r, m_range));
            points.push_back(p1);
            points.push_back(p2);
#ifdef CANDLE_DEBUG
            m_debug[i++].position = p1;
            m_debug[i++].position = p2;
#endif
            rays.pop();
        }
        if(!points.empty()){
            int quads = points.size()/2-1; // a quad between every two rays
            m_polygon.resize(quads * 4);
            for(int i = 0; i < quads; i++){
                float p1 = i*4,  r1 = i*2;
                float p2 = p1+1, r2 = r1+1;
                float p3 = p1+2, r3 = r1+2;
                float p4 = p1+3, r4 = r1+3;
                m_polygon[p1].position = points[r1];
                m_polygon[p2].position = points[r2];
                m_polygon[p3].position = points[r4];
                m_polygon[p4].position = points[r3];
                
                float dr1 = 1.f - m_fade * (sfu::magnitude(points[r2]-points[r1]) / m_range);
                float dr2 = 1.f - m_fade * (sfu::magnitude(points[r4]-points[r3]) / m_range);
                m_polygon[p1].color = m_polygon[p4].color = m_color;
                m_polygon[p2].color = m_polygon[p3].color = m_color;
                m_polygon[p2].color.a = m_color.a * dr1;
                m_polygon[p3].color.a = m_color.a * dr2;
            }
            
        }
        m_transformOfLastCast = Transformable::getTransform();
        m_shouldRecast = false;
    }
}
