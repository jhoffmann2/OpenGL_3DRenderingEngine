/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: classic readme
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_3
Author: Jordan Hoffmann, jordan.h
Creation date: 11/11/2020
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
  - Simply launch application and use the imgui.
    - object window: 
      - rendering component
        - select the mesh of an object
        - toggle rendering
          - modify the meterial parameters
          - modify reflective/refractive parameters
          - toggle between phong/blin lighting/shading
          - modify uv mapping setting
        - toggle vertex normal rendering
          - modify the normal colors
        - toggle face normal rendering
          - modify the normal colors
      - lighting component
        - change light type
        - change light colors
        - change light direction and spotlight parameters
      - Transform Component
        - change position
        - change rotation
        - change rotation pivot
        - change rotation
    - camera window
      - rotate camera
      - change camera's distance from origin
    - world Lighting Window
      - modify world ambient
      - modify fog parameters
      - modify light attenuation
      - modify light count
      - modify light orbit distance
      
    - Menu bar:
      - Load Scenario
        - Scenario 01: randomize the same color and type of ALL lights
        - scenario 02: randomize different color but same type for all lights
        - scenario 03: randomize different color and different type for all lights
  
b. Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
  - the file structure must be maintained when unzipped
  - compiles for x86 only
  - select Assignment_3 as the startup project in visual studio. This solution has all assignments  packed together


c. Which part of the assignment has been completed?
  - All

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?

  - NA

e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).
  all relevant c++ files (that i wrote/modified) are located in CS300/VS_CS300/Assignment_1
  
  - Imgui Menues                              [ Asignement3Scene.cpp line 253 ]
    - Imgui object component menus            [ Solution Filter: GameObject/Component/* ]
  - Environment Map                           [ EnvironmentMap.* ]
  - Skybox Rendering                          [ SkyRender.* ]
  
  
  all shaders are in Common/shaders
  - Phong lighting                            [ PhongLighting.vert ], [ SolidLighting.frag ]
  - Phong shading                             [ SolidShading.vert ], [ PhongShading.frag ]
  - Blinn lighting                            [ BlinnLighting.vert ], [ SolidLighting.frag ]
  - Blinn shading                             [ SolidShading.vert ], [ BlinnShading.frag ]
  
  the following are implementation files included into the above shaders
  - Phong general purpose                     [ phong.glsl ]
  - Blinn general purpose                     [ glinn.glsl ]
  - environmentMap                            [ environmentMap.glsl ]
  - lighting uniforms                         [ lightingUniforms.glsl ]
  - reflection                                [ reflection.glsl ]
  - refraction                                [ refraction.glsl ]
  - uv calculations                           [ uv.glsl ]
  

f. Which machine did you test your application on.
  Remote:
    OS: Microsoft Windows 10 Home
    GPU: GeForce GTX 960M/PCIe/SSE2
    OpenGL Driver Version: 23.20.16.4973
  
g. The number of hours you spent on the assignment, on a weekly basis
  lucky you, i track my time so i have nicely formatted exact data:
  https://wakatime.com/@ddfb8e15-e56f-4db6-a5ed-fa83db741654/projects/iriaurpzgo?start=2020-11-13&end=2020-12-04

  note: this only includes time that i spent coding. any time spent thinking isn't included in this graph.

h. Any other useful information pertaining to the application 
  nope... that should be it.