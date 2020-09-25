<p align="center"><a href=""><img src="doc/logo.svg" alt="logo" height="200px"/></a></p>
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

### Contents

- [Demo](#Demo)
  - [Controls](#Controls)
  - [Build the demo](#Build-the-demo)
- [Requisites](#Requisites)
- 

## Demo

![](doc/img/demo.gif)

Before anything, here you have a little example of how it looks. The code comes with a demo program showing the basic functionalities provided by the library. In it you can place lights and blocks that will cast shadows, and modify the opacity of the fog.

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

#### Build the demo

You can build it with your usual SFML workflow or using the provided Makefile:

```shell
cd /repo/root/path
make # build it
./release/demo # run it
```



### Requisites

![SFML-logo](https://www.sfml-dev.org/download/goodies/sfml-icon.svg)

- **SFML v2.5** 

_This library is meant to be used in SFML applications, so I will assume that you are familiar with the process of compiling them. If you are not, [you can learn in the official website](https://www.sfml-dev.org/tutorials/2.5/)_ . 