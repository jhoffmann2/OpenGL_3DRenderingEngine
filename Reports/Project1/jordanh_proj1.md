<!--Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Author: Jordan Hoffmann, jordan.h
End Header ----------------------------------------------------------->

## [this report is intended to be read from GitHub for proper file links and image rendering](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/blob/master/Reports/Project1/jordanh_proj1.md)

### [Download the latest Release Build Here](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/releases/tag/LatestBuild)

---
## Table Of Contents

- [Project Description](#project-description)
  - [Deferred Rendering](#deferred-rendering)
  - [Localized Lighting](#localized-lighting)
- [Post Mortem](#post-mortem)
- [Preview Images and Videos](#preview-images-and-videos)
  - [Local Lights vs Global Lights](#local-lights-vs-global-lights)
  - [Rendering 1k Local Lights](#rendering-1k-local-lights)
  - [GBuffer](#gbuffer)
- [Code Index](#project-relevant-code-index)
  - [shaders](#relevant-shaders)
  - [c++ code](#relevant-c-code)

---
## Project Description

### Objectives

#### Deferred Rendering

The first goal was to establish a hybrid rendering pipeline. This project works to take a rendering framework capable of forward rendering and transform it into one that supports both forward and deferred rendering. This way we can forward render debug or flat-shaded objects and defer objects with complex lighting.

To accomplish this, I rendered the world geometry into a GBuffer rather than to the screen. This way I could store important geometry info such as position and normal of the nearest fragment in every pixel of the screen and defer it's lighting calculations until after we know exactly which fragment is nearest to the camera.

<img src="GBuffer.png" width="24%"/>

As an experiment, I actually made my GBuffer slightly a-typical. I wanted to challenge myself to cap the memory of the GBuffer in such a way that even if I wanted to add more material properties later, the GBuffer would always stay the same size. Traditionally, 2-4 of the render targets of the GBuffer would be the diffuse, specular, ambient, and emissive colors of the material. This is a ton of textures to pass through the pipeline however and if I ever want to add a new property, bloom for example, I would need to create a whole new target or find a way to compress the data somehow. To solve this, I decided to instead create a 'Materials' SSBO that stores an array of every material used in the scene. This way I can just extend the material struct if I ever want to add interesting properties to materials. The material index is then stored inside the GBuffer as the w component of the world position render target



#### Localized Lighting

The second goal was to extend the above pipeline to support both global and localized lighting. this involved doing a third render pass where I render a sphere at the location of each localized light by accessing the GBuffer and performing an additive lighting calculation on top of the global lighting in that region

<img src="GlobalLightConversion.gif" width="45%"/>
<img src="OneThousandLights.gif" width="45%"/>


---
## Post Mortem
- One of the largest takeaways from this project was experience dealing with FBOs, SSBOs and UBOs. Before working on the deferred rendering, and it's respective material system, I hadn't used any of the following. I'm mostly thankful for my experience with SSBOs and UBOs because I've found many great ways to streamline my shaders by sharing the same data between them inside the buffers. This massively simplified the pipeline.
- The localized lights were also great practice because the same concepts can be applied to multiple other areas of volumetric lighting in the future


---
## Preview Images and Videos

### Local Lights vs Global Lights

<img src="GlobalLightConversion.gif" width="70%"/>

### Rendering 1k Local Lights

<img src="OneThousandLights.gif" width="70%"/>

### GBuffer

<img src="GBuffer.png" width="30%"/>

Q: Where's the specular and diffuse material properties?<br/>
A: rather than putting the material properties in the GBuffer, I uploaded all the materials at once to an SSBO array and indexed into it with the alpha component of the WorldPosition render target.


---
## Project Relevant Code Index

### Relevant Shaders:

#### Render Solids to GBuffer

- [GBuffer.vert](../../Common/shaders/Forward/GBuffer.vert)
  - Includes: [MeshProperties.glsl](../../Common/shaders/Include/MeshProperties.glsl) so the object mesh can be transformed to world space
- [GBuffer.frag](../../Common/shaders/Forward/GBuffer.frag)
  - Includes: [lightingUniforms.glsl](../../Common/shaders/Include/lightingUniforms.glsl) so we can store the material index in the GBuffer
  - Includes: [uv.glsl](../../Common/shaders/Include/uv.glsl) so we can calculate the uv of the diffuse and specular textures and store their color in the GBuffer

#### Render FSQ with GBuffer and Global Lights

- [DeferredPhong.vert](../../Common/shaders/Deferred/DeferredPhong.vert)
  - Includes: [lightingUniforms.glsl](../../Common/shaders/Include/lightingUniforms.glsl) so we can access the material and global light SSBOs and use them in the lighting calculations
- [DeferredPhong.frag](../../Common/shaders/Deferred/DeferredPhong.frag)
  - Includes: [phong.glsl](../../Common/shaders/Include/phongLocal.glsl) so we can perform the lighting calculations

#### Render Local Light with GBuffer

- [LocalLight.vert](../../Common/shaders/Deferred/LocalLight.vert)
  - Includes: [MeshProperties.glsl](../../Common/shaders/Include/MeshProperties.glsl) so the spherical mesh can be transformed to world space
- [LocalLight.frag](../../Common/shaders/Deferred/LocalLight.frag)
  - Includes: [lightingUniforms.glsl](../../Common/shaders/Include/lightingUniforms.glsl) so we can access the material SSBOs and use them in the lighting calculations
  - Includes: [phongLocal.glsl](../../Common/shaders/Include/phongLocal.glsl) so we can calculate the color contribution of the light

### Relevant C++ Code
- deferred shading and local lights: [GBuffer.cpp](../../Projects/RenderingEngine/Rendering/GBuffer.cpp) / [GBuffer.h](../../Projects/RenderingEngine/Rendering/GBuffer.h)
  - Defines, constructs, and manages the GBuffer
  - Methods for binding/unbinding the GBuffer
  - Methods for deferred rendering on to the FSQ
  - Methods for rendering local lights