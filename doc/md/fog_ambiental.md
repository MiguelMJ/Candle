@page fog_ambiental Darkness and ambient light

[TOC]

# Fog and darkness

Light sources alone can become somewhat noisy in the scene. They make much more sense when they exist within an area of darkness, that they illuminate. For such purpose, Candle provides the class candle::LightingArea, that operates in two modes: FOG and AMBIENCE. We will focus in the first one right now. 

A lighting area in FOG mode behaves as a mask, and when objects candle::LightSource are drawn to it, they makes transparent the illuminated polygon, taking into account its intensity and the fade flag. You can think of this class as a wrapper to a sf::RenderTexture, as you have to use clear, draw and display functions. Let's see a minimalistic example:

@include gettingstarted2.cpp

This time the fade flag is set to false to make the contrast stronger. The previous code results in an image hidden by a layer of darkness, revealed under the light of the cursor.

<div align="center">
    <img width="300px" src="example2.gif" alt="Example preview">
    <br><em>Preview</em>
</div>

Also, note that the light is not drawn to the window. If we did that, then the light itself could cover the image below. This doesn't mean that there aren't cases when you will want to draw the light both to the lighting area and the window, but you will have to experiment and adjust the range and intensity parameters, to obtain the desired effect.

## Texturing fog

In the last example we've used plain color to define the fog. However, it is possible to use a texture, instead. In the previous example, we would have to change the piece of code to create the lighting area by the following:

@include gettingstarted3b.cpp

and we would have this result:

<div align="center">
    <img width="300px" src="example3.gif" alt="Example preview">
    <br><em>Preview</em>
</div>

This example also allows us to illustrate how to manage size. An object candle::LightingArea uses internally a sf::RenderTexture, and to avoid destroying and creating repeteadly a potentially heavy resource, it is created only upon construction or when using candle::LightingArea::setAreaTexture. So, if we want to change the size of the area (in this case we want to adjust it to the size of the window), the only way is to scale it.

## Revealing permanently (fog of war effect)

For now we have been calling candle::LightingArea::clear before any draw call. If we don't do this, then the darkness layer isn't restored. This way, we can have the effect of revealing permanently what is under it. 

# Ambient light

The second operation mode of candle::LightingArea is AMBIENT. Its behaviour is rather simple, as it acts as a mere additive layer. Be it a plain color or a texture, they are overlayed to the layer below. Drawing lights in it has no effect, but as light sources are also drawn in an additive manner, then lights within the area will appear to have more intensity.

<table width="100%">
<tr>
<td align="center"> <img src="lightingArea01.png" width="300px"> <br> <em>No LightingArea</em> </td>
<td align="center"> <img src="lightingArea02.png" width="300px"> <br> <em>FOG mode (Color black, medium opacity)</em> </td>
<td align="center"> <img src="lightingArea03.png" width="300px"> <br> <em>AMBIENT mode (Color yellow, low opacity)</em> </td>
</tr>
</table>