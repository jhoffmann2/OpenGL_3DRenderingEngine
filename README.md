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

## [this readme is intended to be read from github for proper file links](https://github.com/jhoffmann2/CS350/blob/master/README.md)

## How to use parts of your user interface that is NOT specified in the assignment description.
This is a non-visual assignment. I've written a unit test project at [VS_CS350/Assignment_2/ntg/ntg.vcxproj](https://github.com/jhoffmann2/CS350/tree/Project2/VS_CS350/Assignment_2/ntg/ntg.vcxproj). You can simply run it from within visual studio.

## Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
- the file structure must be maintained when unzipped
- compiles for x86 only
- select ntg as the startup project in visual studio to run the unit tests or Assignment_2 to run the opengl implemention

## Which part of the assignment has been completed?
- All

## Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
- NA

## Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).
### c++ files
  
- AABB Implementaion: [bounds.h](VS_CS350/Assignment_2/ntg/bounds.h), [bounds.inl](VS_CS350/Assignment_2/ntg/bounds.inl)
- Sphere/Circle Implementaion: [radial.h](VS_CS350/Assignment_2/ntg/radial.h), [radial.inl](VS_CS350/Assignment_2/ntg/radial.inl)
- hyperplane Implementaion: [hyperplane.h](VS_CS350/Assignment_2/ntg/hyperplane.h), [hyperplane.inl](VS_CS350/Assignment_2/ntg/hyperplane.inl)
- ray Implementaion: [ray.h](VS_CS350/Assignment_2/ntg/ray.h), [ray.inl](VS_CS350/Assignment_2/ntg/ray.inl)
- triangle/simplex Implementaion: [simplex.h](VS_CS350/Assignment_2/ntg/simplex.h), [simplex.inl](VS_CS350/Assignment_2/ntg/simplex.inl)
- collision: [collision.h](VS_CS350/Assignment_2/ntg/collision.h), [collision.inl](VS_CS350/Assignment_2/ntg/collision.inl)
- unit tests: [test.cpp](VS_CS350/Assignment_2/ntg/test.cpp)

## Which machine did you test your application on.
Remote:
  OS: Microsoft Windows 10 Home
  GPU: GeForce GTX 960M/PCIe/SSE2
  OpenGL Driver Version: 23.20.16.4973
  
## The number of hours you spent on the assignment, on a weekly basis
lucky you, i track my time so i have nicely formatted exact data:
https://wakatime.com/@ddfb8e15-e56f-4db6-a5ed-fa83db741654/projects/xvmtqczpqw?start=2021-01-31&end=2021-02-21

note: this only includes time that i spent coding. any time spent thinking isn't included in this graph.

## Any other useful information pertaining to the application 
nope... that should be it.
