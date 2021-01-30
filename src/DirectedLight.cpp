#include "Candle/DirectedLight.hpp"

#include <algorithm>

#include "sfml-util/geometry/Vector2.hpp"
#include "sfml-util/graphics/VertexArray.hpp"

namespace candle{
    void DirectedLight::draw(sf::RenderTarget& t, sf::RenderStates st) const{
        st.transform *= Transformable::getTransform();
        t.draw(m_polygon, st);
    }
    
    DirectedLight::DirectedLight(){
        m_polygon.setPrimitiveType(sf::Quads);
        m_polygon.resize(2);
        setBeamInclination(0.f);
        setBeamWidth(10.f);
        castLight();
    }
    
    void DirectedLight::setRange(float range){
        m_range = range;
        m_shouldRecast = true;
    }
    
    void DirectedLight::setBeamWidth(float width){
        m_beamWidth = width;
        m_shouldRecast = true;
    }
    
    float DirectedLight::getBeamWidth() const{
        return m_beamWidth;
    }
    
    void DirectedLight::setBeamInclination(float angle){
        m_beamInclination = angle;
        m_shouldRecast = true;
    }
    
    float DirectedLight::getBeamInclination() const{
        return m_beamInclination;
    }
    
    void DirectedLight::castLight(){
        struct LineParam {
            sfu::Line line;
            float param;
            LineParam(const sfu::Line& l, float p)
                : line(l)
                {
                line = l;
                param = p;
            }
            LineParam(const sf::Vector2f& orig, const sf::Vector2f& dir, float p)
                : line(orig, orig + dir)
                {
                param = p;
            }
        };
        sf::Transform trm(
            1, 0, 0,
            m_beamInclination, 1, 0,
            0, 0, 1
        );
        trm *= Transformable::getTransform();
        sf::Transform trm_i = trm.getInverse();
        
        float widthHalf = m_beamWidth/2.f;
        sf::FloatRect baseBeam(0, -widthHalf, m_range, m_beamWidth);
        
        sf::Vector2f lim1o = trm.transformPoint(0, -widthHalf);
        sf::Vector2f lim2o = trm.transformPoint(0, widthHalf);
        sf::Vector2f lim1d = trm.transformPoint(m_range, -widthHalf);
        sf::Vector2f lim2d = trm.transformPoint(m_range, widthHalf);
        
        float off = 0.01/sfu::magnitude(lim2o - lim1o);
        sf::Vector2f lightDir = lim1d - lim1o;
        
        sfu::Line lim1(lim1o, lim1d);
        sfu::Line lim2(lim2o, lim2d);
        sfu::Line raySrc(lim1o, lim2o);
        sfu::Line rayRng(lim1d, lim2d);
        
        std::vector<LineParam> rays;
        int s = 0;
        for(auto& pool : m_ptrSegmentPool){
            s += pool->size();
        }
        rays.reserve(2 + s * 2 * 3); // 2: beam angle, 2: pnts/sgmnt, 3 rays/pnt
        
        rays.emplace_back(lim1, 0.f);
        rays.emplace_back(lim2, 1.f);
        
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
                    rays.emplace_back(raySrc.point(tRng), lightDir, tRng);
                }
                float t;
                if(baseBeam.contains(trm_i.transformPoint(seg.m_origin))){
                    raySrc.intersection(sfu::Line(seg.m_origin, -lim1.m_direction), t);
                    rays.emplace_back(raySrc.point(t - off), lightDir, t - off);
                    rays.emplace_back(raySrc.point(t), lightDir, t);
                    rays.emplace_back(raySrc.point(t + off), lightDir, t + off);
                }
                if(baseBeam.contains(trm_i.transformPoint(seg.point(1.f)))){
                    raySrc.intersection(sfu::Line(seg.point(1.f), -lim1.m_direction), t);
                    rays.emplace_back(raySrc.point(t - off), lightDir, t - off);
                    rays.emplace_back(raySrc.point(t), lightDir, t);
                    rays.emplace_back(raySrc.point(t + off), lightDir, t + off);
                }
            }
        }
        
        std::sort(rays.begin(),
                  rays.end(),
                  [&](LineParam& lp1, LineParam& lp2){
                      return lp1.param < lp2.param;
                  }
                 );
        
        std::vector<sf::Vector2f> points;
        points.reserve(rays.size()*2);
#ifdef CANDLE_DEBUG
        int deb_r = rays.size()*2 + 6;
        m_debug.resize(deb_r);
        sfu::setColor(m_debug, sf::Color::Magenta);
        int i=0;
        m_debug[deb_r-1].color = m_debug[deb_r-2].color = sf::Color::Cyan;
        m_debug[deb_r-3].color = m_debug[deb_r-4].color = sf::Color::Yellow;
        m_debug[deb_r-2].position = {0, -widthHalf};
        m_debug[deb_r-4].position = {0, widthHalf};
#endif
        for(auto& r: rays){
            sf::Vector2f p1 = trm_i.transformPoint(r.line.m_origin);
            sf::Vector2f p2 = trm_i.transformPoint(castRay(r.line));
            points.push_back(p1);
            points.push_back(p2);
#ifdef CANDLE_DEBUG
        m_debug[i++].position = p1;
        m_debug[i++].position = p2;
#endif
        }
        size_t quads = rays.size()-1; // a quad between every two rays
        m_polygon.resize(quads * 4);
        for(size_t i = 0; i < quads; i++){
            m_polygon[quads*4].position = points[quads*2];
            m_polygon[quads*4+1].position = points[quads*2+1];
            m_polygon[quads*4+2].position = points[quads*2+3];
            m_polygon[quads*4+3].position = points[quads*2+2];
        }
        
        m_transformOfLastCast = Transformable::getTransform();
        m_shouldRecast = false;
    }
}
