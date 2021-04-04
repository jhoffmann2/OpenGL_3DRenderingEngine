
/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GLApplication.cpp
Purpose: entry point to the program - initializes glfw, OpenGL, and ImGui
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Local / project headers
#include "../Common/Scene.h"
#include "shader.hpp"
#include "MainScene.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include "imgui.h"

// Function declarations
bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);

//////////////////////////////////////////////////////////////////////

GLFWwindow *window;
Scene  *scene;

int windowWidth = 1280*2;
int windowHeight = 720*2;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    // Setting up OpenGL properties
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
                              "CS350 Jordan Hoffmann", // window title
                              nullptr, // which monitor (if full-screen mode)
                              nullptr); // if sharing context with another window
    if (window == nullptr)
    {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Setup ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
    ImGui::StyleColorsDark();
    //ImGui::GetStyle().ScaleAllSizes(4.f);

    // give imgui the window to render to
    if(!ImGui_ImplGlfw_InitForOpenGL(window, true))
      throw "imgui failed to initialize for glfw and opengl";
    ImGui_ImplOpenGL3_Init("#version 460");

    // Initialize the scene
    scene = new MainScene( windowWidth, windowHeight );

    // Scene::Init encapsulates setting up the geometry and the texture
    // information for the scene
    scene->Init();

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    do
    {

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Now render the scene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        scene->Display();

        // Render Imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // Swap buffers
        glfwSwapBuffers(window);

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    ImGui_ImplGlfw_Shutdown();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height)
{
    std::ofstream  texFile(filepath);

    texFile << "P3" << std::endl;
    texFile << width << "  " << height << std::endl;
    texFile << "255" << std::endl;

    auto it = pixels.begin();

    for( int row = 0; row < height; ++row )
    {
        for (int col = 0; col < width; ++col)
        {
            texFile << *it++ << " ";
            texFile << *it++ << " ";
            texFile << *it++ << " ";
        }

        texFile << std::endl;
    }

    texFile.close();

    return true;
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  dynamic_cast<MainScene*>(scene)->KeyCallback(window, key, scancode, action, mods);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  dynamic_cast<MainScene*>(scene)->MouseCallback(window, xpos, ypos);
}