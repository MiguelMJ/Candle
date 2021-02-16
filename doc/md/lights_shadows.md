@page lights_shadows Lights and shadows

 [TOC]

# Introduction

The two main objects related to shadow casting in Candle are light sources and edges.

For the lights, there is an abstract class called candle::LightSource, implemented by candle::RadialLight and candle::DirectedLight. They inherit for sf::Drawable and sf::Transformable, so they can be used as the more common SFML entities.

For the edges, there is a structure called [**candle::Edge**](LightSource_8hpp.html#a232f746b6098d9a876d23e84a67dc0a2), that can represent a segment by the coordinates of its ends. However, the raycasting function requires that the shadow casting edges are stored in a `std::vector`, so Candle provides a convenient alias for `std::vector<candle::Edge>` called [**candle::EdgeVector**](LightSource_8hpp.html#a384b0e96a22f34b27f84a55713279e89). This is what you should be using to manage your edges. 

They interact through the function candle::LightSource::castLight, that takes two iterators from a [**candle::EdgeVector**](LightSource_8hpp.html#a384b0e96a22f34b27f84a55713279e89) and use the edges contained between them to compute the area illuminated by the light source.

Let's see with a sample of code.

@include gettingstarted1.cpp

The previous code results in a light that follows the mouse and a vertical edge in the center of the screen.

<div align="center"><img src="example.gif" width="250px"><br><em>Preview</em></div>

Note how the `castLight` function is called only when the mouse is moved. Although it shouldn't cost much when a light has a normal amount of edges in range, is preferable not to abuse it unnecesarily. Therefore, we will call it only when the light has  been modified or the edges in range have moved.

# Radial light and Directed light

In the previous example we have used a candle::RadialLight. This is the light type that casts rays in any direction from a single point. The other type is candle::DirectedLight, that casts rays in a single direction, from any point within a segment.

<table width="100%">
<tr>
<td align="center"> <img src="radial_2.png" width="300px"> <br> <em>Radial lights</em> </td>
<td align="center"> <img src="directed_2.png" width="300px"> <br> <em>Directed lights</em> </td>
</tr>
</table>
# Problems with intersections

Candle doesn't behave well when segments intersect. The reason is that lights project only to the end of segments, so intersection points, that are within the edge, are ignored.

<table width="100%">
<tr>
<td align="center"> <img width="300px" src="intersection_1.png" alt="Intersection of edges example"> <br> <em>First case: Intersection between edges:</em> </td>
<td align="center"> <img width="300px" src="intersection_2.png" alt="Intersection of edge and directed light example"> <br> <em>Second case: Intersection between an edge and the source of a directed light</em> </td>
</tr>
</table>
You should avoid this when disposing the edges and lights in the scene.

# Customizing the lights

There are four common parameters to customize light sources, and one parameter specific of each child class.

## Common parameters

### Intensity


Alpha component of the light, managed separately from the plain color.
- candle::LightSource::getIntensity
- candle::LightSource::setIntensity

<div align="center">
<img width="300px" src="param_intensity_1.png" alt="Intensity preview">    
<br><em>Top left: hight intensity. Bottom right: low intensity.</em>
</div>
### Color

Plain color of the light.

- candle::LightSource::getColor
- candle::LightSource::setColor

<div align="center">
    <img width="300px" src="param_color_1.png" alt="Color preview">
    <br><em>Top left: Color cyan. Bottom right: Color yellow.</em>
</div>
### Range

Max range of the iluminated area.

- candle::LightSource::getRange
- candle::LightSource::setRange

<div align="center">
    <img width="300px" src="param_range_1.png" alt="Range preview">
    <br><em>Top left: Low range. Bottom right: High range.</em>
</div>
### Fade

Flag that indicates if the light fades towards the range limit.

- candle::LightSource::getFade
- candle::LightSource::setFade

<div align="center">
    <img width="300px" src="param_fade_1.png" alt="Fade preview">
    <br><em>Top left: Fade off. Bottom right: Fade on.</em>
</div>
## RadialLight parameters

### Beam angle

Angle (in degrees) that limits the difference between the angles of the casted rays and the rotation of the light.

- candle::RadialLight::getBeamAngle
- candle::RadialLight::setBeamAngle

<div align="center">
    <img width="300px" src="param_beamangle_1.png" alt="Beam angle preview">
    <br><em>Top left: 90º. Top right: 180º. Bottom left: 270º. Bottom right: 360º.</em>
</div>
## DirectedLight parameters

### Beam width

Maximum allowed distance from the center of the segment to a cast point.

- candle::DirectedLight::getBeamWidth
- candle::DirectedLight::setBeamWidth

<div align="center">
    <img width="300px" src="param_beamwidth_1.png" alt="Beam width preview">
    <br><em>Left: narrow beam. Right: wide beam.</em>
</div>
​	
