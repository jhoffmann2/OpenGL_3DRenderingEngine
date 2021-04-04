/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Language: MSVC c++17
Platform: win64 compiled in Visual Studio 2019
Project: jordan.h_CS350_3
Author: Jordan Hoffmann, jordan.h
Creation date: 04/04/2021
End Header --------------------------------------------------------*/ 

## [this readme is intended to be read from github for proper file links](https://github.com/jhoffmann2/CS350/blob/master/README.md)

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
- rotate camera (note you can also modify these values with WASD)
- change camera's distance from origin (note you can also modify this value with QE)
### world Lighting Window
- modify world ambient
- modify fog parameters
- modify light attenuation
- modify light count
- modify light orbit distance
### Deferred Shading window
- toggle copying depth buffer
- view render targets
### Spacial Tree Hierarchy window
- Power Plant Octree
  - open the drop-down header's in this gui to view explore the tree
  - sections of the power plant which fall under the open nodes will be outlined to match the color displayed in ImGui
  - hover mouse over the powerplant to raycast onto the powerplant
- Section# BspTree
  - open the drop-down header's in this gui to view explore the tree
  - sections of the power plant which fall under the open nodes will be outlined to match the color displayed in ImGui
  - there will be one BspTree for every power plant section that you've loaded
- The Spacial Tree Hierarchy will automatically adjust in real time if you modify the power plant's transform
### Menu bar:
- Power Plant
  - Toggle sections of the powerplant (warning slow loading meshes first time you load a section)

## Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
- the file structure must be maintained when unzipped
- compiles for x64 only

## Which part of the assignment has been completed?
- All

## Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
- NA

## Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).

- Spacial Tree Heirarchy: [SpacialTree.h](VS_CS350/RenderingEngine/SpacialTree.h), [SpacialTree.cpp](VS_CS350/RenderingEngine/SpacialTree.cpp)
- Octrees: [Octree.h](VS_CS350/RenderingEngine/Octree.h), [Octree.cpp](VS_CS350/RenderingEngine/Octree.cpp)
- BSP Trees: [BspTree.h](VS_CS350/RenderingEngine/BspTree.h), [BspTree.cpp](VS_CS350/RenderingEngine/BspTree.cpp)
- Debug Drawing: [DebugDraw.h](VS_CS350/RenderingEngine/DebugDraw.h), [DebugDraw.cpp](VS_CS350/RenderingEngine/DebugDraw.cpp)

## Which machine did you test your application on.
Remote:
  OS: Microsoft Windows 10 Home
  GPU: GeForce GTX 960M/PCIe/SSE2
  OpenGL Driver Version: 23.20.16.4973
  
## The number of hours you spent on the assignment, on a weekly basis
lucky you, i track my time so i have nicely formatted exact data:
https://wakatime.com/@ddfb8e15-e56f-4db6-a5ed-fa83db741654/projects/xvmtqczpqw?start=2021-03-13&end=2021-04-04

note: this only includes time that i spent coding. any time spent thinking isn't included in this graph.

## Any other useful information pertaining to the application 
nope... that should be it.
