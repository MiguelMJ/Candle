# Contributing to Candle

Thanks for thinking about contributing to Candle! Here are the steps to do it.

1. Read this document (you are on the right path in this moment).
2. Check if there's any open issue with your idea, and whether it is already assigned to someone or not.
3. If it is not, then you can open a new issue or comment in the existent one and I will assign it to you.
4. Fork the repo and make your changes in the dev branch.
5. Make a pull request.

## Where is help wanted?

This is a list of topics that could make it to a future version of Candle.

- **Use of shaders**. I'm afraid I have zero practical knowledge about programming shaders. If any of the functionalities achieved using blending modes and prerendered textures (the case of RadialLight.cpp) can be replicated with shaders, that would be awesome.

  - Not only replicating the already available effects, but also adding new ones like penumbra, blur, more complex fading functions...

  Relevant files: `src/RadialLight.cpp`, `src/DirectedLight.cpp`, `src/LightingArea.cpp`

- **Optimization to raycasting algorithms**. This algorithms have already several optimizations, but I don't know if it is possible to speed them up more. 

  Relevant fiels: `src/Line.hpp`, `src/RadialLight.cpp`, `src/DirectedLight.cpp`

  There is only one case where I know it would be possible but I have not been able to make it work: the comprobation that a RadialLight makes in `src/RadialLight.cpp:142` is intended to filter out edges whose mathematical line definition is far away. It is better than no filter, but it would be better to use the distance to the mathematical line `src/RadialLight.cpp:138` only when the orthogonal projection is contained within the edge segment, and otherwise use the minimum between the distance to the ends of the edge (see commented in `src/RadialLight.cpp:141`).

  Use of several technologies I'm still not very familiar with.

## Where is help NOT wanted?

Unless they are heavily justified, the following changes will not be considered.

- Adding functions that could be replaced with two lines of user code.
- Changes in the demo.

***

I hope you find Candle interesting and useful.

Credit is not required, but it would be fantastic if you mentioned it if you use it in a project.

Also, you can give Candle some visibility with a  :star:  **star in this repository** ![](https://img.shields.io/github/stars/MiguelMJ/Candle?style=social). 