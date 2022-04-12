<!--Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Author: Jordan Hoffmann, jordan.h
End Header ----------------------------------------------------------->

## [this report is intended to be read from GitHub for proper file links and image rendering](https://github.com/jhoffmann2/OpenGL_3DRenderingEngine/blob/master/Reports/Project3/jordanh_proj3.md)

---
## Table Of Contents

- [Project Description](#project-description)
  - [Ambient Occlusion](#ambient-occlusion)
    - [Edge Aware Blur](#edge-aware-blur)
- [Post Mortem](#post-mortem)
- [Preview Images](#preview-images)
- [Code Index](#project-relevant-code-index)
  - [shaders](#relevant-shaders)
  - [c++ code](#relevant-c-code)

---
## Project Description
For this project I implemented a Screen Space Ambient Occlusion algorithm that samples nearby fragments in the GBuffer to calculate the probability that a fragment is in shadow from the environment. I then scaled the incoming ambient light from the environment light developed in the last project by the ambient occlusion factor.

### Ambient Occlusion
To calculate the ambient occlusion factor of a fragment, we select a series of pseudorandom points along a spiral centered at the fragment, and check the GBuffer to determine whether the sampled point is shadowing the fragment. The more sampled points that shadow the fragment, the darker the incoming light is.

![](AO_Noise.png)

#### Edge Aware Blur
To get a crisp image with the above method, we'd need to sample an unrealistically large number of points. To avoid the performance bottleneck, the spiral sampling intentionally samples different points at nearby pixels. This increases the amount of output noise, but because nearby points collectively hold more info this way, we can blur them to mix their collective information while reducing noise. Unlike a standard Gaussian blur however, we have to do an edge aware blur. Ideally, the blur would only blur across nearby fragments with similar depths and normals since those fragments are more likely to have the same ambient occlusion. Luckily all this info is stored in the GBuffer, so we can scale a fragments contribution to th weighted blur average by its "similarity" in depth and normal.

![](AO_Blurred.png)

---
## Post Mortem
All in all, this project is relatively pain free provided that you already have a robust deferred rendering pipeline. Of all the feature's I've added to this rendering engine, SSAO was definitely the biggest bang for buck. The level of realism that proper ambient occlusion adds is huge! It mixes with the previous BRDF/Environment Light project beautifully and even if I weren't going for realism, SSAO is a great trick for stylized rendering too.

---
## Preview Images

| Without Ambient Occlusion | With Ambient Occlusion |
|:-------------------------:|:----------------------:|
| ![](NoAO.gif)             | ![](AO.gif)            |

| Adjusting Ambient Occlusion Settings |
|:------------------------------------:|
| ![](AOSettings.gif)                  |


---
## Project Relevant Code Index

### Relevant Shaders

#### Rendering the Ambient Occlusion Map
- [AmbientOcclusionPass.vert](../../Common/shaders/Deferred/AmbientOcclusionPass.vert)
- [AmbientOcclusionPass.frag](../../Common/shaders/Deferred/AmbientOcclusionPass.frag)

#### Edge/Depth Aware Blur Shader
- [EdgeAwareGaussianBlurX.comp](../../Common/shaders/Compute/EdgeAwareGaussianBlurX.comp)
- [EdgeAwareGaussianBlurY.comp](../../Common/shaders/Compute/EdgeAwareGaussianBlurY.comp)

### Relevant C++ Code
- [GBuffer.cpp](../../RenderingEngine/Rendering/GBuffer.cpp)
  - Added code for rendering the Ambient Occlusion Map to the GBuffer and then blurring it
