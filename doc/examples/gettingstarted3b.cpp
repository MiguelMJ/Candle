    // create the lighting area
    sf::Texture img2;
    if(!img2.loadFromFile("starry_night_van_gogh.png")){
        exit(1);
    }
    candle::LightingArea fog(candle::LightingArea::FOG,
                             &img2);
    fog.setScale((float)w.getSize().x / img2.getSize().x,
                  (float)w.getSize().y / img2.getSize().y);
