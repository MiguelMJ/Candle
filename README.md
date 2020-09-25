<p align="center"><a href="miguelmj.github.io/Candle"><img src="doc/logo.svg" alt="logo" height="200px"/></a></p>
<h1 align="center">Candle</h1>
<h3 align="center">2D lighting for SFML</h3>
<p align="center">
<img src="https://img.shields.io/badge/C++-11-informational">
<img src="https://img.shields.io/badge/SFML-2.5-informational">
<a href="https://miguelmj.github.io/Candle">
    <img src="https://img.shields.io/badge/code-documented-success"/>
</a>
<a href="https://github.com/MiguelMJ/Candle">
    <img src="https://img.shields.io/github/repo-size/MiguelMJ/Candle"/>
</a>
<a href="LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-informational"/>
</a>
</p>


Candle is a SFML based C++ library that provides light, shadow casting and field of view  functionalities with easy integration.

### Contents

- [Demo](#Demo)
  - [Controls](#Controls)
  - [Build](#Build)
- [Requisites](#Requisites)
- [Example program](#Example-program)
- [Contributing](#Contributing)

## Demo

Before anything, here you have a little example of how it looks.

<p align="center"><img src="doc/img/demo.gif" height="450"></p>

The code comes with a demo program showing the basic functionalities provided by the library. In it you can place lights and blocks that will cast shadows, and modify the opacity of the fog.

#### Controls

- **Right click**. Alternate between block and light.
- **Left click**. Put a block or light.
- **Spacebar**. Erase all lights and blocks. 

- **Mouse wheel**. Increase / Decrease light radius.

- **G**. Toggle glow.
- **C**. Alternate glow color between white, magenta, yellow and cyan. 
- **A** / **S**. Increase / Decrease light intensity. 

- **Z** / **X**. Increase / Decrease fog opacity.

- **P**. Capture  the window content and save it to a png file in the current working directory.

#### Build

You can build it with your usual SFML workflow or using the provided Makefile:

```shell
cd /repo/root/path
make prep
make # build it
./release/demo # run it
```

## Requisites

<img src="https://www.sfml-dev.org/download/goodies/sfml-icon.svg" height="50">

- **SFML v2.5** 
  - Graphics module and System module.

_This library is meant to be used in SFML applications, so I will assume that you are familiar with the process of compiling them. If you are not, [you can learn in the official website](https://www.sfml-dev.org/tutorials/2.5/)_ . 

## Example program

I will assume that you have SFML installed in your system. If we have a project with the following structure:

```
|- project
   |- main.cpp
   |- Candle # this repository
      |- include
      |- src
      | ...
```

the `main.cpp` file could look like this:

```C++
#include <SFML/Graphics.hpp>
#include "Candle/Lighting.hpp"

int main(){
    // create window
    sf::RenderWindow w(sf::VideoMode(400,400), "app");
    
    // create the lighting area
	candle::Lighting lighting;
    lighting.setFogOpacity(0.6);
    lighting.adjustFog(w.getView());
    lighting.updateFog();
    
    // create the light and a shadow to cast
    candle::LightSource light;
    light.setRadius(150);
    lighting.addLightSource(&light);
    lighting.m_segmentPool.push_back({{200,100},{200,300}});
    
    // main loop
    while(w.isOpen()){
        sf::Event e;
        while(w.pollEvent(e)){
            if(e.type == sf::Event::Closed){
                w.close();
            }else if(e.type == sf::Event::MouseMoved){
                // make the light follow the mouse
                sf::Vector2i mpi = sf::Mouse::getPosition(w);
                light.setPosition(sf::Vector2f(mpi.x,mpi.y));
                light.castLight();
            }
        }
        
        w.clear();
        w.draw(lighting);
        w.display();
    }
	return 0;
}

```

We can compile it with the following command:

```shell
g++ -std=c++11 -o app -ICandle/include Candle/src/* main.cpp -lsfml-graphics -lsfml-window -lsfml-system
```

And we run it

```shell
./app
```

The result will be a simple light casting a shadow over an invisible wall in the center of the window.

<p align="center"><img src="doc/img/example.gif" height="300"/></p>

### Contributing

Here's a To Do list for Candle:

- Currently this library needs more testing and documentation.

  - Performance tests - this library is not yet optimized.
  - Functionality tests - there may be bugs I haven't found yet.
  - Tutorials and examples.
  - Documentation review - [the documentation](https://miguelmj.github.io/Candle) still needs some more writing and maybe spell checking.

- The functionalities of the library are enough for a basic lighting system, but there is still room for more complex behaviours.

  - If you want to request some feel free to open a new issue or even implement it and make a pull request.

- The integration is simple, but it would be even simpler with a better build system.

- Finally, you can still star this repository and give it some visibility :star:

  