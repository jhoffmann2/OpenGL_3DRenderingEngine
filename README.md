/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS350_1
Author: Jordan Hoffmann, jordan.h
Creation date: 01/29/2021
End Header --------------------------------------------------------*/ 

##[this readme is intended to be read from github for proper file links](https://github.com/jhoffmann2/CS350/blob/master/README.md)

## How to use parts of your user interface that is NOT specified in the assignment description.
Simply launch application and use the imgui.
### object window: 
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
- rotate camera
- change camera's distance from origin
### world Lighting Window
- modify world ambient
- modify fog parameters
- modify light attenuation
- modify light count
- modify light orbit distance
### Menu bar:
- Power Plant
  - Toggle sections of the powerplant (warning slow loading meshes first time you load a section)

## Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
- the file structure must be maintained when unzipped
- compiles for x86 only
- select Assignment_3 as the startup project in visual studio. This solution has all assignments  packed together


## Which part of the assignment has been completed?
- All

## Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?

- NA

## Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).
### c++ files
  
- [ImGui Menus](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/Assignment1Scene.cpp#L380-L473)
  - [LightComponent](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/LightComponent.cpp#L40-L167)
  - [RenderingComponent](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/RenderingComponent.cpp#L74-L174)
  - [TransformComponent](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/TransformComponent.cpp#L32-L58)
  - [MaterialComponent](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/MaterialComponent.cpp#L17-L52)
- Loading Power Plant
  - [mesh loading](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/Mesh.cpp#L281-L348)
  - [power plant section loading](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/Assignment1Scene.cpp#L113-L141)
- [Deferred Rendering](https://github.com/jhoffmann2/CS350/blob/e2606d9109065344dee46761770b43e532e9e7d3/VS_CS350/Assignment_1/GBuffer.cpp#L12-L221)

### Shaders
#### Forward Rendering:
- Phong lighting: [vertex shader](Common/shaders/Forward/PhongLighting.vert), [fragment shader](Common/shaders/Forward/PhongLighting.frag)
- Phong shading:  [vertex shader](Common/shaders/Forward/PhongShading.vert), [fragment shader](Common/shaders/Forward/PhongShading.frag)
- Blinn lighting: [vertex shader](Common/shaders/Forward/BlinnLighting.vert), [fragment shader](Common/shaders/Forward/BlinnLighting.frag)
- Blinn shading:  [vertex shader](Common/shaders/Forward/BlinnShading.vert), [fragment shader](Common/shaders/Forward/BlinnShading.frag)
- Flat emission:  [vertex shader](Common/shaders/Forward/FlatEmission.vert), [fragment shader](Common/shaders/Forward/FlatEmission.frag)

#### Deferred Rendering:
- First Pass:  [vertex shader](Common/shaders/Forward/GBuffer.vert), [fragment shader](Common/shaders/Forward/GBuffer.frag)
- Second Pass: [vertex shader](Common/shaders/Deferred/DeferredPhong.vert), [fragment shader](Common/shaders/Deferred/DeferredPhong.frag)

### Helper shader files
- [Phong general purpose](Common/shaders/Include/phong.glsl)
- [Blinn general purpose](Common/shaders/Include/blinn.glsl)
- [Lighting Uniforms](Common/shaders/Include/lightingUniforms.glsl)
- [UV Calculations](Common/shaders/Include/uv.glsl)
- [Mesh Properties](Common/shaders/Include/MeshProperties.glsl)

## Which machine did you test your application on.
Remote:
  OS: Microsoft Windows 10 Home
  GPU: GeForce GTX 960M/PCIe/SSE2
  OpenGL Driver Version: 23.20.16.4973
  
## The number of hours you spent on the assignment, on a weekly basis
lucky you, i track my time so i have nicely formatted exact data:
https://wakatime.com/@ddfb8e15-e56f-4db6-a5ed-fa83db741654/projects/xvmtqczpqw?start=2021-01-09&end=2021-01-31

note: this only includes time that i spent coding. any time spent thinking isn't included in this graph.

## Any other useful information pertaining to the application 
nope... that should be it.