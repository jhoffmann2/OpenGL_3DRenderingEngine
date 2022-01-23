<!--Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Project: jordan.h_CS350_3
Author: Jordan Hoffmann, jordan.h
Creation date: 04/04/2021
End Header ----------------------------------------------------------->

## [this readme is intended to be read from github for proper file links](https://github.com/jhoffmann2/CS350/blob/master/README.md)

## How to use parts of your user interface that is NOT specified in the assignment description

Simply launch application and use the imgui. 
(note: this demo uses a large mesh and performs preprocessing on it so give the program several minutes to launch)

### object window

- rendering component
  - select the mesh of an object
  - toggle rendering
    - select solid rendering shader
  - toggle vertex normal rendering
    - modify the normal colors
  - toggle face normal rendering
    - modify the normal colors
- material component
  - modify phong lighting components
  - if the rendering component shader is set to "Local Light", the material component properties will instead store the emission properties of the light
- lighting component
  - change light type
  - change light colors
  - change light direction and spotlight parameters
- Transform Component
  - change position
  - change scale
  - change pivot
  - change rotation
- ChildListComponent
  - see child objects and edit their components
  - note: if parent has a component that the child doesn't have, the child will inherit those properties

### camera window

- rotate camera (note you can also modify these values with WASD)
- change camera's distance from origin (note you can also modify this value with QE)

### world Lighting Window

- modify world ambient light
- modify fog parameters
- modify light attenuation
- modify light count
- modify light orbit distance

### Deferred Shading window

- toggle copying depth buffer
- view render targets

### Spacial Tree Hierarchy window

(note: irrelevant to this assignment)
- Power Plant Octree
  - open the drop-down header's in this gui to view explore the tree
  - sections of the power plant which fall under the open nodes will be outlined to match the color displayed in ImGui
  - hover mouse over the powerplant to raycast onto the powerplant
- Section# BspTree
  - open the drop-down header's in this gui to view explore the tree
  - sections of the power plant which fall under the open nodes will be outlined to match the color displayed in ImGui
  - there will be one BspTree for every power plant section that you've loaded
- The Spacial Tree Hierarchy will automatically adjust in real time if you modify the power plant's transform

### Menu bar

- Power Plant
  - Toggle sections of the powerplant (warning slow loading meshes first time you load a section)
- LightConversion
  - Allows the user to easily toggle lights between global mode and local mode. Because this involves modifying several components at once, this speeds up the process

## Any assumption that you make on how to use the application that, if violated, might cause the application to fail

- the file structure must be maintained when unzipped
- compiles for x64 
- build with cmake and the msvc compiler

## Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number)

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
- deferred shading and local lights: [GBuffer.cpp](Projects/RenderingEngine/GBuffer.cpp) / [GBuffer.h](Projects/RenderingEngine/GBuffer.h)
  - Defines, constructs, and manages the GBuffer
  - Methods for binding/unbinding the GBuffer
  - Methods for deferred rendering on to the FSQ
  - Methods for rendering local lights

## Preview Images
### Local Lights vs Global Lights
![Demonstrating the visual difference between a global light and a local one](D:\Digipen\2020-2021\CS350\Previews\GlobalLightConversion.gif "Local vs Global Lights")

## Which machine did you test your application on
- OS: Microsoft Windows 10 Home 64-bit
- GPU: NVIDIA GeForce RTX 3080 Laptop GPU
- OpenGL Driver Version: 30.0.14.9729
