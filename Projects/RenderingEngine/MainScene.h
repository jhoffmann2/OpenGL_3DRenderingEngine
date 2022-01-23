/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Assignment1Scene.h
Purpose: the main scene where everything in assignment 1 is displayed
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#ifndef SIMPLE_SCENE_SIMPLESCENE_Obj_H
#define SIMPLE_SCENE_SIMPLESCENE_Obj_H

#include "../Common/Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <bitset>
#include <vector>

#include "ntg/bounds.h"
#include "SolidRender.h"
#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "ParentChildComponent.h"


class MainScene : public Scene
{
public:
  MainScene() = default;
  MainScene( int windowWidth, int windowHeight );
  virtual ~MainScene();

  void SetupNanoGUI(GLFWwindow* pWwindow) override{}


public:
  int Init() override;
  void CleanUp() override;

  int Render() override;
  int postRender() override;
  void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:

  // member functions
  void initMembers();

  ntg::bounds3 ActivePowerPlantBounds() const;
  void UpdateActivePowerPlants();
  [[nodiscard]] const std::unordered_set<ParentChildComponent*>& PowerPlantGroup() const;

  std::vector<GameObject*> lights_;
  int activeLightCount_ = 3;

  
  GLfloat angleOfRotation;
  size_t viewportWidth_;
  size_t viewportHeight_;
  Camera cam;

  // camera controls:
  float x_polar = 0;
  float y_polar = 20;
  float radius = 10.f;

  enum BaseObjects
  {
    POWER_PLANT = 0,
    LIGHTS = 1,
    BaseObjectsCount
  };

  static constexpr size_t powerPlantGroupCount = 21;
  //std::vector<ntg::bounds3> powerPlantBounds_;
  glm::mat4 powerPlantTransformation_;

  std::vector<GameObject*> objects_;
  glm::vec4 mouse_ndc;
};


#endif //SIMPLE_SCENE_SIMPLESCENE_Obj_H
