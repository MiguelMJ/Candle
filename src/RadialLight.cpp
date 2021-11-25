#ifdef CANDLE_DEBUG
#include <iostream>
#endif

#include <map>
#include <memory>
#include "Candle/RadialLight.hpp"

#include "SFML/Graphics.hpp"

#include "Candle/graphics/VertexArray.hpp"
#include "Candle/geometry/Vector2.hpp"
#include "Candle/geometry/Line.hpp"

namespace candle{
    int RadialLight::s_instanceCount = 0;
    const unsigned int BASE_RADIUS = 400;
    bool l_texturesReady(false);
    std::map<sf::Texture*, std::pair<std::unique_ptr<sf::Texture>, std::unique_ptr<sf::Texture> >> l_lightTexturesCache;
    std::unique_ptr<sf::Texture> l_lightTextureFade;
    std::unique_ptr<sf::Texture> l_lightTexturePlain;

    void initializeTextures(){
        #ifdef CANDLE_DEBUG
        std::cout << "RadialLight: InitializeTextures" << std::endl;
        #endif

        sf::Image lightImageFade;
        sf::Image lightImagePlain;

        //We create an image of the radius*2 + an 2 more pixels
        lightImageFade.create(BASE_RADIUS*2+2, BASE_RADIUS*2+2, sf::Color(255,255,255,0));
        lightImagePlain.create(BASE_RADIUS*2+2, BASE_RADIUS*2+2, sf::Color(255,255,255,0));

        sf::Vector2f center = {BASE_RADIUS, BASE_RADIUS};

        for (unsigned int y=1; y<BASE_RADIUS*2; ++y){
            for (unsigned int x=1; x<BASE_RADIUS*2; ++x){
                //Get distance of the pixel from the center, normalize it with the radius and inverse it
                float distanceFromCenter = 1.0f - sfu::magnitude(sf::Vector2f(x,y) - center) / BASE_RADIUS;

                if ( distanceFromCenter < 0.0f ){
                    //Too far from center, pixel will be fully transparent
                    lightImageFade.setPixel(x,y, sf::Color(255,255,255,0));
                    lightImagePlain.setPixel(x,y, sf::Color(255,255,255,0));
                }else{
                    lightImageFade.setPixel(x,y, sf::Color(255,255,255, static_cast<unsigned int>(255.0f*distanceFromCenter)));
                    lightImagePlain.setPixel(x,y, sf::Color(255,255,255,255));
                }
            }
        }

        l_lightTextureFade.reset( new sf::Texture() );
        l_lightTextureFade->loadFromImage(lightImageFade);
        l_lightTextureFade->setSmooth(true);

        l_lightTexturePlain.reset( new sf::Texture() );
        l_lightTexturePlain->loadFromImage(lightImagePlain);
        l_lightTexturePlain->setSmooth(true);
    }

    void cleanupTexture(sf::Texture* texture){
        if (texture != nullptr){
            //Custom texture
            auto cachedTextureIter = l_lightTexturesCache.find(texture);

            if (cachedTextureIter != l_lightTexturesCache.end()){
                l_lightTexturesCache.erase(cachedTextureIter);
            }
        }else{
            //Every texture
            l_lightTexturesCache.clear();
        }
    }

    float module360(float x){
        x = (float)fmod(x,360.f);
        if(x < 0.f) x += 360.f;
        return x;
    }

    RadialLight::RadialLight()
        : LightSource()
        {
        if(!l_texturesReady){
            // The first time we create a RadialLight, we must create the textures
            initializeTextures();
            l_texturesReady = true;
        }

        setTexture(nullptr);

        setRange(1.0f);
        setBeamAngle(360.f);
        // castLight();
        s_instanceCount++;

        setColor(sf::Color(255,255,255,255));
    }

    RadialLight::~RadialLight(){
        s_instanceCount--;
        #ifdef RADIAL_LIGHT_FIX
        if (s_instanceCount == 0 &&
            l_lightTextureFade &&
            l_lightTexturePlain)
        {
            l_lightTextureFade.reset(nullptr);
            l_lightTexturePlain.reset(nullptr);
            l_texturesReady = false;
            #ifdef CANDLE_DEBUG
            std::cout << "RadialLight: Textures destroyed" << std::endl;
            #endif
        }
        #endif
    }

    void RadialLight::draw(sf::RenderTarget& t, sf::RenderStates s) const{
        sf::Transform trm = Transformable::getTransform();
        trm.scale(m_range/m_localRadius, m_range/m_localRadius, m_localRadius, m_localRadius);
        s.transform *= trm;
        s.texture = m_fade ? m_lightTextureFade : m_lightTexturePlain;
        if(s.blendMode == sf::BlendAlpha){
            s.blendMode = sf::BlendAdd;
        }
        t.draw(m_polygon, s);
#ifdef CANDLE_DEBUG
        sf::RenderStates deb_s;
        deb_s.transform = s.transform;
        t.draw(m_debug, deb_s);
#endif
    }
    void RadialLight::resetColor(){
        sfu::setColor(m_polygon, m_color);
    }

    void RadialLight::setBeamAngle(float r){
        m_beamAngle = module360(r);
    }

    float RadialLight::getBeamAngle() const{
        return m_beamAngle;
    }

    void RadialLight::setTexture(sf::Texture* texture){
        if (texture != nullptr){
            //Custom texture
            m_lightTexturePlain = texture;
            auto cachedTextureIter = l_lightTexturesCache.find(texture);

            if (cachedTextureIter != l_lightTexturesCache.end()){
                m_lightTexturePlain = cachedTextureIter->second.first.get();
                m_lightTextureFade = cachedTextureIter->second.second.get();
            }else{
                sf::Image userTexture = texture->copyToImage();

                unsigned int diameter = ((userTexture.getSize().x > userTexture.getSize().y) ? userTexture.getSize().x : userTexture.getSize().y) + 2;

                //Put 1px transparent border to avoid bad clamp
                sf::Image plainImage;
                sf::Image fadeImage;
                fadeImage.create(diameter, diameter, sf::Color(255,255,255,0));
                plainImage.create(diameter, diameter, sf::Color(255,255,255,0));

                sf::Vector2f center = sf::Vector2f(diameter/2.0f, diameter/2.0f);

                for (unsigned int y=0; y<diameter; ++y)
                {
                    for (unsigned int x=0; x<diameter; ++x)
                    {
                        sf::Color pixel = userTexture.getPixel(x%userTexture.getSize().x,y%userTexture.getSize().y);
                        plainImage.setPixel(x+1,y+1, pixel);

                        //Get distance of the pixel from the center, normalize it with the radius and inverse it
                        float distanceFromCenter = 1.0f - sfu::magnitude(sf::Vector2f(x,y) - center) / (diameter/2.0f);

                        if ( distanceFromCenter < 0.0f ){
                            //Too far from center, pixel will be fully transparent
                            fadeImage.setPixel(x+1,y+1, sf::Color(255,255,255,0));
                        }else{
                            pixel.a = static_cast<unsigned int>(255.0f*distanceFromCenter);
                            fadeImage.setPixel(x+1,y+1, pixel);
                        }
                    }
                }

                sf::Texture* plainTexture = new sf::Texture();
                sf::Texture* fadeTexture = new sf::Texture();
                plainTexture->loadFromImage(plainImage);
                fadeTexture->loadFromImage(fadeImage);
                plainTexture->setSmooth(true);
                fadeTexture->setSmooth(true);

                l_lightTexturesCache[texture] = std::make_pair(std::unique_ptr<sf::Texture>(plainTexture), std::unique_ptr<sf::Texture>(fadeTexture));

                m_lightTexturePlain = plainTexture;
                m_lightTextureFade = fadeTexture;
            }
        }else{
            //Default texture
            m_lightTexturePlain = l_lightTexturePlain.get();
            m_lightTextureFade = l_lightTextureFade.get();
        }

        m_localRadius = m_lightTexturePlain->getSize().x/2+1;

        m_polygon.setPrimitiveType(sf::TriangleFan);
        m_polygon.resize(6);
        m_polygon[0].position =
        m_polygon[0].texCoords = {static_cast<float>(m_localRadius), static_cast<float>(m_localRadius)};
        m_polygon[1].position =
        m_polygon[5].position =
        m_polygon[1].texCoords =
        m_polygon[5].texCoords = {0.f, 0.f};
        m_polygon[2].position =
        m_polygon[2].texCoords = {static_cast<float>(m_localRadius)*2, 0.f};
        m_polygon[3].position =
        m_polygon[3].texCoords = {static_cast<float>(m_localRadius)*2, static_cast<float>(m_localRadius)*2};
        m_polygon[4].position =
        m_polygon[4].texCoords = {0.f, static_cast<float>(m_localRadius)*2};
        Transformable::setOrigin(m_localRadius, m_localRadius);
    }

    sf::Texture* RadialLight::getLightFadeTexture(){
        return m_lightTextureFade;
    }

    sf::Texture* RadialLight::getLightPlainTexture(){
        return m_lightTexturePlain;
    }

    sf::FloatRect RadialLight::getLocalBounds() const{
        return sf::FloatRect(0.0f, 0.0f, m_localRadius*2, m_localRadius*2);
    }

    sf::FloatRect RadialLight::getGlobalBounds() const{
        float scaledRange = m_range / m_localRadius;
        sf::Transform trm = Transformable::getTransform();
        trm.scale(scaledRange, scaledRange, m_localRadius, m_localRadius);
        return trm.transformRect( getLocalBounds() );
    }

    void RadialLight::castLight(const EdgeVector::iterator& begin, const EdgeVector::iterator& end){
        float scaledRange = m_range / m_localRadius;
        sf::Transform trm = Transformable::getTransform();
        trm.scale(scaledRange, scaledRange, m_localRadius, m_localRadius);
        std::vector<sfu::Line> rays;

        rays.reserve(2 + std::distance(begin, end) * 2 * 3); // 2: beam angle, 4: corners, 2: pnts/sgmnt, 3 rays/pnt

        // Start casting
        float bl1 = module360(getRotation() - m_beamAngle/2);
        float bl2 = module360(getRotation() + m_beamAngle/2);
        bool beamAngleBigEnough = m_beamAngle < 0.1f;
        auto castPoint = Transformable::getPosition();
        float off = .001f;

        auto angleInBeam = [&](float a)-> bool {
            return beamAngleBigEnough
                   ||(bl1 < bl2 && a > bl1 && a < bl2)
                   ||(bl1 > bl2 && (a > bl1 || a < bl2));
        };

        for(float a = 45.f; a < 360.f; a += 90.f){
            if(beamAngleBigEnough || angleInBeam(a)){
                rays.emplace_back(castPoint, a);
            }
        }

        sf::FloatRect lightBounds = getGlobalBounds();
        for(auto it = begin; it != end; it++){
            auto& s = *it;

            //Only cast a ray if the line is in range
            if( lightBounds.intersects( s.getGlobalBounds() ) ){
                sfu::Line r1(castPoint, s.m_origin);
                sfu::Line r2(castPoint, s.point(1.f));
                float a1 = sfu::angle(r1.m_direction);
                float a2 = sfu::angle(r2.m_direction);
                if(angleInBeam(a1)){
                    rays.push_back(r1);
                    rays.emplace_back(castPoint, a1 - off);
                    rays.emplace_back(castPoint, a1 + off);
                }
                if(angleInBeam(a2)){
                    rays.push_back(r2);
                    rays.emplace_back(castPoint, a2 - off);
                    rays.emplace_back(castPoint, a2 + off);
                }
            }
        }

        if(bl1 > bl2){
            std::sort(
                rays.begin(),
                rays.end(),
                [bl1, bl2] (sfu::Line& r1, sfu::Line& r2){
                    float _bl1 = bl1-0.1;
                    float _bl2 = bl2+0.1;
                    float a1 = sfu::angle(r1.m_direction);
                    float a2 = sfu::angle(r2.m_direction);
                    return (a1 >= _bl1 && a2 <= _bl2) || (a1 < a2 && (_bl1 <= a1 || a2 <= _bl2));
                }
            );
        }else{
            std::sort(
                rays.begin(),
                rays.end(),
                [bl1] (sfu::Line& r1, sfu::Line& r2){
                    return
                        sfu::angle(r1.m_direction) < sfu::angle(r2.m_direction);
                }
            );
        }
        if(!beamAngleBigEnough){
            rays.emplace(rays.begin(), castPoint, bl1);
            rays.emplace_back(castPoint, bl2);
        }

        sf::Transform tr_i = trm.getInverse();
        // keep only the ones within the area
        std::vector<sf::Vector2f> points;
        points.reserve(rays.size());
        for (auto& r: rays){
            points.push_back(tr_i.transformPoint(castRay(begin, end, r, m_range*2)));
        }
        m_polygon.resize(points.size() + 1 + beamAngleBigEnough); // + center and last
        m_polygon[0].color = m_color;
        m_polygon[0].position = m_polygon[0].texCoords = tr_i.transformPoint(castPoint);
#ifdef CANDLE_DEBUG
        float bl1rad = bl1 * sfu::PI/180.f;
        float bl2rad = bl2 * sfu::PI/180.f;
        sf::Vector2f al1(std::cos(bl1rad), std::sin(bl1rad));
        sf::Vector2f al2(std::cos(bl2rad), std::sin(bl2rad));
        int d_n = points.size()*2 + 4;
        m_debug.resize(d_n);
        m_debug[d_n-1].color = m_debug[d_n-2].color = sf::Color::Cyan;
        m_debug[d_n-3].color = m_debug[d_n-4].color = sf::Color::Yellow;
        m_debug[d_n-1].position = m_debug[d_n-3].position = m_polygon[0].position;
        m_debug[d_n-2].position = tr_i.transformPoint(castPoint + m_range * al1);
        m_debug[d_n-4].position = tr_i.transformPoint(castPoint + m_range * al2);
#endif
        for(unsigned i=0; i < points.size(); i++){
            sf::Vector2f p = points[i];
            m_polygon[i+1].position = p;
            m_polygon[i+1].texCoords = p;
            m_polygon[i+1].color = m_color;
#ifdef CANDLE_DEBUG
            m_debug[i*2].position = m_polygon[0].position;
            m_debug[i*2+1].position = p;
            m_debug[i*2].color = m_debug[i*2+1].color = sf::Color::Magenta;
#endif
        }
        if(beamAngleBigEnough){
            m_polygon[points.size()+1] = m_polygon[1];
        }
    }

}
