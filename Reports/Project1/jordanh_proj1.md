<!--Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Author: Jordan Hoffmann, jordan.h
End Header ----------------------------------------------------------->

## [this report is intended to be read from github for proper file links and image rendering](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/blob/master/Reports/Project1/jordanh_proj1.md)

### [Download the latest Release Build Here](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/releases/tag/LatestBuild)

---
## Table Of Contents

- ###[Preview Images](#preview-images)
  - [Local Lights vs Global Lights](#local-lights-vs-global-lights)
  - [Rendering 1k Local Lights](#rendering-1k-local-lights)
  - [GBuffer](#gbuffer)
- ###[Code Index](#project-relevant-code-index)
  - [shaders](#relevant-shaders)
  - [c++ code](#relevant-c-code)

---
## Preview Images

### Local Lights vs Global Lights

![Demonstrating the visual difference between a global light and a local one](Reports/Project1/GlobalLightConversion.gif "Local vs Global Lights")

### Rendering 1k Local Lights

![Rendering 1000 local lights](Reports/Project1/OneThousandLights.gif "Stress Testing 1000 global lights")

### GBuffer

![Debug view of the GBuffer](Reports/Project1/GBuffer.png "GBuffer")

Q: Where's the specular and diffuse material properties?<br/>
A: rather than putting the material properties in the gbuffer, I uploaded all the materials at once to an SSBO array and indexed into it with the alpha component of the WorldPosition render target.


---
## Project Relevant Code Index

### Relevant Shaders:

#### Render Solids to GBuffer

- [GBuffer.vert](Common/shaders/Forward/GBuffer.vert)
  - Includes: [MeshProperties.glsl](Common/shaders/Include/MeshProperties.glsl) so the object mesh can be transformed to world space
- [GBuffer.frag](Common/shaders/Forward/GBuffer.frag)
  - Includes: [lightingUniforms.glsl](Common/shaders/Include/lightingUniforms.glsl) so we can store the material index in the GBuffer
  - Includes: [uv.glsl](Common/shaders/Include/uv.glsl) so we can calculate the uv of the diffuse and specular textures and store their color in the GBuffer

#### Render FSQ with GBuffer and Global Lights

- [DeferredPhong.vert](Common/shaders/Deferred/DeferredPhong.vert)
  - Includes: [lightingUniforms.glsl](Common/shaders/Include/lightingUniforms.glsl) so we can access the meterial and global light SSBOs and use them in the lighting calculations
- [DeferredPhong.frag](Common/shaders/Deferred/DeferredPhong.frag)
  - Includes: [phong.glsl](Common/shaders/Include/phongLocal.glsl) so we can perform the lighting calculations

#### Render Local Light with GBuffer

- [LocalLight.vert](Common/shaders/Deferred/LocalLight.vert)
  - Includes: [MeshProperties.glsl](Common/shaders/Include/MeshProperties.glsl) so the spherical mesh can be transformed to world space
- [LocalLight.frag](Common/shaders/Deferred/LocalLight.frag)
  - Includes: [lightingUniforms.glsl](Common/shaders/Include/lightingUniforms.glsl) so we can access the meterial SSBOs and use them in the lighting calculations
  - Includes: [phongLocal.glsl](Common/shaders/Include/phongLocal.glsl) so we can calculate the color contribution of the light

### Relevant C++ Code
- deferred shading and local lights: [GBuffer.cpp](Projects/RenderingEngine/Rendering/GBuffer.cpp) / [GBuffer.h](Projects/RenderingEngine/Rendering/GBuffer.h)
  - Defines, constructs, and manages the GBuffer
  - Methods for binding/unbinding the GBuffer
  - Methods for deferred rendering on to the FSQ
  - Methods for rendering local lights