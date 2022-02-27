<!--Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Author: Jordan Hoffmann, jordan.h
End Header ----------------------------------------------------------->

## [this report is intended to be read from GitHub for proper file links and image rendering](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/blob/master/Reports/Project2/jordanh_proj2.md)

---
## Table Of Contents

- [Project Description](#project-description)
  - [Shadows](#shadows)
  - [Probabilistic Shadow Blurring](#probabilistic-shadow-blurring)
- [Post Mortem](#post-mortem)
- [Preview Images and Videos](#preview-images-and-videos)
  - [Shadow Depth Map](#shadow-depth-map)
  - [ShadowMap Lookup](#shadowmap-lookup)
  - [Shadows](#shadows)
- [Code Index](#project-relevant-code-index)
  - [shaders](#relevant-shaders)
  - [c++ code](#relevant-c-code)

---
## Project Description

### Objectives

#### Shadows

As a first step I had to create a camera at the light casting shadows, and forward render the scene geometry through a fragment shader that output vec4(depth, depth^2, depth^3, depth^4) to a texture in the GBuffer
(note that we won't use depth^2, depth^3, or depth^4 until we add blurring)

<img src="ShadowMap.gif" width="40%"/>

Later, in the deferred shader, we can use the light's camera matrix to do a reverse lookup and get the minimum light depth of a fragment's world position. This looks as follows

<img src="ShadowLookup.gif" width="40%"/>

And finally, by comparing the above depth with the actual camera depth, we can determine whether the fragment is in shadow or not

<img src="Shadow_Blur_00.gif" width="40%"/>


#### Probabilistic Shadow Blurring

The issue with the above approach however, is that there's a hard edge between what's shadowed and lit. This feels unrealistic and has the added downside of bad aliasing. To counter this, we Ideally would like to blur the shadows

To do this, the depth texture is passed through two gaussian blur compute shaders (one for x and one for y)
<img src="ShadowMap_blurred.gif" width="40%"/>

Now it's important to note that simply blurring the depth does not blur the shadows. Because your fragment shader makes a binary decision between "in shadow" or not, there will still be a hard edge, I'll be it rounded and slightly less noisy.

To counter this, we have to do some statistics using all four components of the blurred shadow map and solve for the probability that the fragment is in shadow. Conveniently, probability will give us a smooth gradient between zero and one rather than a binary edge

Shadow Blur Radius = 02  | Shadow Blur Radius = 10  |  Shadow Blur Radius = 64
:-----------------------:|:------------------------:|:------------------------:
![](Shadow_Blur_02.gif)  | ![](Shadow_Blur_10.gif)  |  ![](Shadow_Blur_64.gif)


---
## Post Mortem
This project was the first time I've ever implemented shadows in general, let alone blurred shadows. It was a great experience and I learned a ton. I ended up spending the majority of my time getting the blurring to work because it had a lot more moving parts. I had to write a compute shader, something which I had very limited experience in, and I had to convert that into a probability that the fragment is in shadow. All in all, it was a good experience but very hard work.


---
## Preview Images and Videos

### Shadow Depth Map

<img src="ShadowMap.gif" width="60%"/>

# ShadowMap Lookup

<img src="ShadowLookup.gif" width="60%"/>

### Shadows

Shadow Blur Radius = 00  |  Shadow Blur Radius = 02
:-----------------------:|:-------------------------:
![](Shadow_Blur_00.gif)  |  ![](Shadow_Blur_02.gif)
Shadow Blur Radius = 10  |  Shadow Blur Radius = 64
![](Shadow_Blur_10.gif)  |  ![](Shadow_Blur_64.gif)


---
## Project Relevant Code Index

### Relevant Shaders:

#### Render Depth Map

- [DepthMap.vert](../../Common/shaders/Forward/DepthMap.vert)
  - Includes: [MeshProperties.glsl](../../Common/shaders/Include/MeshProperties.glsl) so the object mesh can be transformed to world space
  - Includes: [LightDepth.glsl](../../Common/shaders/Include/lightDepth.glsl) so we can normalize the depth based on the scene scale
- [DepthMap.frag](../../Common/shaders/Forward/DepthMap.frag)
  - simply outputs vec4(depth, depth^2, depth^3, depth^4)

#### Blur Depth Map

- [GaussianBlurX.comp](../../Common/shaders/Compute/GaussianBlurX.comp)
- [GaussianBlurY.comp](../../Common/shaders/Compute/GaussianBlurY.comp)

#### Render Shadow

- [phong.glsl](../../Common/shaders/Include/phong.glsl) 
(included in any shader doing phong lighting)
  - convert world position into light camera ndc
  - get the nearest depth at position from shadow depth map
  - compare actual depth with shadow depth by solving cholesky equation
  - add light by 1-P(fragment is in shadow)

### Relevant C++ Code
- [GBuffer.cpp](../../RenderingEngine/Rendering/GBuffer.cpp) / [GBuffer.h](../../RenderingEngine/Rendering/GBuffer.h)
  - Includes shadowMap as one of the GBuffer render targets
  - Functionality to blur a texture using the Gaussian blur compute shaders
- [LightComponent.cpp](../../RenderingEngine/Lighting/LightComponent.cpp)
  - Functionality to render the shadow map for a light
