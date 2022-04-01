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
  - [BRDF Lighting](#brdf-lighting)
  - [Image Based Lighting](#image-based-lighting)
    - [Irradiance Map](#irradiance-map)
    - [Monte Carlo Specular Sampling](#monte-carlo-specular-sampling)
- [Post Mortem](#post-mortem)
- [Preview Images](#preview-images)
- [Code Index](#project-relevant-code-index)
  - [shaders](#relevant-shaders)
  - [c++ code](#relevant-c-code)

---
## Project Description
For this project I converted my lighting shaders from Phong shading to more physically based lighting known as Bidirectional Reflectance Distribution Function (BRDF) shading. This allowed me to use HDR environment maps as image based lights to make objects look like they're actually in the environment being rendered in

### BRDF Lighting
Like phong, BRDF can be broken into it's specular and diffuse components. Diffuse is very similar to phong, while specular combines the following three concepts to better model the world
- A Distribution function: this models the fraction of the surface that's aligned with the viewing angle and the incoming light
- A Fresnel function: this models the fraction of light that gets reflected rather than absorbed by a material. This allows for materials to become more reflective when viewed at a shallow angle
- A self occlusion function: this function recognizes that rough surfaces are subtly darker at extreme light angles because the micro-facets occlude one another from the light

To create my BRDF model, I followed this [PDF](BRDF.pdf)

### Image Based Lighting
rather than clamping color values between 0 and 1, HDR images have floating point values between 0 and infinity. This allows them to model light by not clamping the color of the sun to the same color as say, a white piece of paper. we can then take an HDR environment map, and use it to calculate the light coming in from the environment as if it were thousands of tiny, pixel sized, lights.

#### Irradiance Map
the diffuse component of BRDF can be simplified to use an irradiance map to generalize the incoming light from that direction. The irradiance map can be pre-generated but the naive approach is a n^2 algorithm because every pixel in the environment map has to consider the influence of every other pixel. Instead, we can use spherical harmonics to generalize how the color changes as u and v change in terms of a second degree polynomial. because polynomials can be evaluated in constant time, it's a O(n) algorithm to generate the polynomial, and a O(n) algorithm to evaluate that polynomial at every pixel in the irradiance map both done in series and therefore making a O(n) algorithm. Much more manageable for a pre-processing step and conveniently multithread friendly!


![img.png](environment_map.png) | ![img.png](irradiance_map.png)
:------------------------------:|:--------------------------------:
Environment Map                 | Irradiance Map

#### Monte Carlo Specular Sampling
unfortunately since specular light varies between mirror-like reflection and scattered light, there's no trick for preprocessing incoming specular light like there is for diffuse with irradiance maps. instead, I sample several directions in the environment and composite them into a final specular color for each fragment. The directions are selected by picking uniformly distributed, pseudo random numbers, mapping them into vectors whose distribution match the probability of a micro-facet reflecting light from the eye in that direction (as determined by the material's specular roughness constant). As a final step, we use mip-mapping to blur the samples taken from rough surfaces.

---
## Post Mortem
This project proved to be really difficult for me, unfortunately. I had some flaws in my specular sampling model and it caused a lot of really weird artifacts. additionally, my BRDF model caused an unexpected white banding around objects. I found a solution but the whole project burned a ton of time. Generating the irradiance maps was relatively painless in comparison, however.

---
## Preview Images

### Results at different specular roughness

![](WhiteDiffuseOnly.png) | ![](WhiteDiffuse_SpecExp005.png) | ![](WhiteDiffuse_SpecExp020.png) | ![](WhiteDiffuse_SpecExp080.png) | ![](WhiteDiffuse_SpecExp320.png) | ![](WhiteDiffuse_SpecExp1280.png) |
:------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|
Diffuse Lighting Only     |  Smoothness = 0005               |  Smoothness = 0020               |  Smoothness = 0080               |  Smoothness = 0320               |  Smoothness = 1280               |

### Results in different environments

![](alexs_apt.png) | ![](barce_rooftop.png) | ![](hamarikyu_bridge.png) | ![](moon_valley.png) | ![](nature_lab.png) | ![](newport_loft.png) |
:------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|:--------------------------------:|

---
## Project Relevant Code Index

### Relevant Shaders:

#### BRDF Lighting

- [DeferredBRDF.vert](../../Common/shaders/Deferred/DeferredBRDF.vert)
- [DeferredBRDF.frag](../../Common/shaders/Deferred/DeferredBRDF.frag)
  - Includes: [brdf.glsl](../../Common/shaders/Include/brdf.glsl) to perform BRDF lighting calculations
  - Includes: [toneMapping.glsl](../../Common/shaders/Include/toneMapping.glsl) to tone map the output into normalized RGB

### Relevant C++ Code
- [IrradianceMap.cpp](../../RenderingEngine/Utilities/IrradianceMap.cpp)
  - Generating an Irradiance map with a given HDR environment texture
