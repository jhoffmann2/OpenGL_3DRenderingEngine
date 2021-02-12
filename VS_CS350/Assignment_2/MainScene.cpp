/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Assignment1Scene.cpp
Purpose: the main scene where everything in assignment 1 is displayed
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/
#include "MainScene.h"


#include <array>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/easing.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "imgui.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "Utilities.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/random.hpp"
#include "LightSystem.h"
#include "MaterialSystem.h"
#include "RenderingComponent.h"
#include "TransformComponent.h"
#include "VertexGlobalSystem.h"
#include "FaceNormalRender.h"
#include "GBuffer.h"
#include "MaterialComponent.h"
#include "ChildListComponent.h"
#include "imgui_internal.h"
#include "VertexNormalRender.h"

#define DEFERRED true
#define LOG_ACTIVE true


const glm::vec3 O = { 0, 0, 0 },
                EX = { 1, 0, 0 },
                EY = {0, 1, 0},
                EZ = {0, 0, 1};

const glm::vec3 BLACK(0, 0, 0),
                WHITE(1, 1, 1),
                REDISH(1, 0.6f, 0.6f),
                GREENISH(0.6f, 1, 0.6f),
                BLUEISH(0.6f, 0.6f, 1);


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
MainScene::~MainScene()
{
  initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
static const glm::vec3 cam_pos{ 0.f,.75f,3.75f};

MainScene::MainScene(int windowWidth, int windowHeight) :
  Scene(windowWidth, windowHeight),
  angleOfRotation(0.0f),
  windowWidth_(windowWidth),
  windowHeight_(windowHeight),
  cam(
    cam_pos, 
    -cam_pos, 
    EY, 
    60.f, 
    static_cast<float>(windowWidth) / 
          static_cast<float>(windowHeight), 
    0.01f, 
    100
  )
{
  initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void MainScene::initMembers()
{
  angleOfRotation = 0.0f;
  objects_.clear();
}

glm::bounds3 MainScene::ActivePowerPlantBounds()
{
  glm::bounds3 out;
  for(size_t i = 0; i < powerPlantGroupCount; ++i)
  {
    if(PowerPlantGroupObjects()[i]->IsActive())
    {
      out = out.grow(powerPlantBounds_[i]);
    }
  }
  return out;
}

void MainScene::UpdateActivePowerPlants()
{
  constexpr size_t lerp_time = 60; // lerp over 120 frames
  static size_t cur_lerp = lerp_time + 1;

  for (auto* obj : PowerPlantGroupObjects())
  {
    if(obj->IsActive() && !obj->HasComponent<ChildListComponent>())
    {
      std::vector<Mesh> meshes;
      std::vector<std::string> names;

      ImportMeshList(
        "../../Common/models/Powerplant/",
        obj->Name() + ".txt",
        meshes,
        names
      );

      std::cout << "centering and normalizing meshes" << std::endl;
      MeshTransform(meshes, powerPlantTransformation_);

      obj->AddComponent(
        new ChildListComponent(meshes, names)
      );
      cur_lerp = lerp_time + 1;
      break;
    }
  }
  
  static glm::bounds3 prev_bounds;
  glm::bounds3 cur_bounds = ActivePowerPlantBounds();
  if(cur_bounds != prev_bounds)
  {
    cur_lerp = lerp_time + 1;
    prev_bounds = cur_bounds;
  }

  // update the pivot so powerplant is centered;
  static glm::vec3 destination_pivot;
  static glm::vec3 start_pivot;
  auto* transform = objects_[0]->GetComponent<TransformComponent>();
  if(cur_lerp > lerp_time)
  {
    cur_lerp = 1;

    glm::bounds3 bounds = ActivePowerPlantBounds();
    glm::vec3 center = ActivePowerPlantBounds().center();
    center.y = bounds.min.y + 0.25f * bounds.size().y; // favor looking toward the ground

    glm::vec3 pivot = -(powerPlantTransformation_ * glm::vec4(center, 1));
    destination_pivot = pivot * transform->GetScale();
    start_pivot = transform->GetPivot();
  }

  float t = glm::sineEaseInOut(static_cast<float>(cur_lerp) / lerp_time);
  transform->SetPivot(mix(start_pivot, destination_pivot, t));
  ++cur_lerp;
}

const std::vector<GameObject*>& MainScene::PowerPlantGroupObjects() const
{
  return objects_[POWER_PLANT]->GetComponent<ChildListComponent>()->Children();
}

const std::vector<GameObject*>& MainScene::LightObjects() const
{
  return objects_[LIGHTS]->GetComponent<ChildListComponent>()->Children();
}

ElementRange<const std::vector<GameObject*>> MainScene::ActiveLights() const
{
  const auto& allLights = LightObjects();
  return { allLights.begin(), allLights.begin() + activeLightCount_ };
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void MainScene::CleanUp()
{
  for(GameObject *obj : objects_)
  {
    delete obj;
  }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int MainScene::Init()
{
  GBuffer::Init(windowWidth_, windowHeight_);

  VertexGlobalSystem::Init();

  const auto sphere_meshData(generateSphereMesh(30));

  const int sphere_mesh = SolidRender::loadMesh(sphere_meshData, SolidRender::PHONG_SHADING);
  VertexNormalRender::loadMesh(sphere_meshData);
  FaceNormalRender::loadMesh(sphere_meshData);


  if(std::filesystem::exists("../../Common/models/Powerplant/bounds.txt"))
  {
    std::ifstream boundsFile("../../Common/models/Powerplant/bounds.txt");
    while(!boundsFile.eof())
    {
      powerPlantBounds_.emplace_back();
      boundsFile >> powerPlantBounds_.back();
    }
    powerPlantBounds_.pop_back();
  }
  else
  {
    std::cout << "calculating power plant size" << std::endl;
    for (size_t i = 1; i <= powerPlantGroupCount; ++i)
    {
      powerPlantBounds_.emplace_back();
      const std::string name = "Section" + std::to_string(i);

      std::vector<Mesh> meshes;
      std::vector<std::string> names;

      ImportMeshList(
        "../../Common/models/Powerplant/",
        name + ".txt",
        meshes,
        names
      );

      const auto categoryBounds = meshBounds(meshes);
      powerPlantBounds_.back() = powerPlantBounds_.back().grow(categoryBounds);
    }

    std::ofstream boundsFile("../../Common/models/Powerplant/bounds.txt");
    glm::bounds3 fullPlantBounds;
    for(const auto& b : powerPlantBounds_)
    {
      fullPlantBounds = fullPlantBounds.grow(b);
      boundsFile << b << std::endl;
    }
    powerPlantBounds_.emplace_back(fullPlantBounds);
    boundsFile << fullPlantBounds << std::endl;
  }

  objects_.emplace_back(new GameObject("Power Plant"));
  objects_.back()->AddComponent(
    new TransformComponent(
      { 0,0,0 },
      { 0,0,0 },
      { EY,0 },
      40.f
    )
  );
  
  MaterialHandle material(0);
  material.SetEmissiveColor(glm::vec3{ 0 });
  material.SetAmbientColor(glm::vec3{ .0742f });
  material.SetDiffuseColor(glm::vec3{ .6392f,.5412f,.4118f });
  material.SetSpecularColor(glm::vec3{ 0.3176f });
  material.SetSpecularExponent(20);
  objects_.back()->AddComponent(new MaterialComponent(material));
  auto* pc = new ChildListComponent();

  powerPlantTransformation_ = CenterMeshTransform(powerPlantBounds_.back());
  for (size_t i = 1; i <= powerPlantGroupCount; ++i)
  {
    const std::string name = "Section" + std::to_string(i);
    auto* section = new GameObject(name);
    section->SetActive(false);
    pc->AddChild(section);
  }
  objects_.back()->AddComponent(pc);

  // preload section 10
  {
    auto* obj = PowerPlantGroupObjects()[9];
    obj->SetActive(true);
    std::vector<Mesh> meshes;
    std::vector<std::string> names;

    ImportMeshList(
      "../../Common/models/Powerplant/",
      obj->Name() + ".txt",
      meshes,
      names
    );

    std::cout << "centering and normalizing meshes" << std::endl;
    MeshTransform(meshes, powerPlantTransformation_);

    obj->AddComponent(
      new ChildListComponent(meshes, names)
    );
  }

  {
    objects_.emplace_back(new GameObject("Lights"));
    auto* pc = new ChildListComponent();
    objects_.back()->AddComponent(pc);

      const float hue = glm::linearRand(24.f,40.f);
    MaterialHandle light_material(1);
    light_material.SetEmissiveColor(rgbColor(glm::vec3{ hue, 0.0f, 1.f }));
    light_material.SetAmbientColor(glm::vec3{ 0 });
    light_material.SetDiffuseColor(glm::vec3{ 0 });
    light_material.SetSpecularColor(glm::vec3{ 0 });
    light_material.SetSpecularExponent(20);

    for(size_t i = 0; i < LightSystem::lightCount; ++i)
    {
      std::stringstream name;
      name << "Light #" << i;

      auto *lightObj = new GameObject(name.str());
      lightObj->AddComponent(
        new TransformComponent(
          {0,0,0},
          {2,0,0},
          {EY,0},
          .06f
        )
      );

      auto* light = new LightComponent(i);
      light->SetDiffuseColor(rgbColor(glm::vec3{hue, .25f, 1.f}));
      light->SetSpecularColor(rgbColor(glm::vec3{ hue, .1f, 1.f }));
      light->SetAmbientColor(rgbColor(glm::vec3{ hue, .25f, 0.2f }));
      light->SetType(Light::POINT);
      light->SetActive(true);
      lightObj->AddComponent(light);

      auto* rendering = new RenderingComponent(sphere_mesh, SolidRender::FLAT_EMISSION);
      lightObj->AddComponent(rendering);

      lightObj->AddComponent(new MaterialComponent(light_material));

      lightObj->SetActive(false);

      pc->AddChild(lightObj);
    }
  }

  for(auto *light : ActiveLights())
  {
    light->SetActive(true);
  }

  VertexNormalRender::setCamera(cam);
  FaceNormalRender::setCamera(cam);

  LightSystem::Init();
  LightSystem::SetAmbientColor({ .4078f, .6627f,1.f });
  LightSystem::SetLightAttenuation({ 0.f, .2f, .02f });
  LightSystem::SetFog({.6039f, .6157f,.6549f}, 0, 40);

  MaterialSystem::Init();

  return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int MainScene::Render()
{

  UpdateActivePowerPlants();

  if(ImGui::Begin("Camera"))
  {
    ImGui::PushItemWidth(400);

    ImGui::SliderFloat("X rotation", &x_polar, -180, 180, "%.1f degrees");
    ImGui::SliderFloat("Y rotation", &y_polar, -89.9f, 89.9f, "%.1f degrees");
    ImGui::DragFloat("Radius", &radius, 0.002f);
    glm::vec3 direction{ 1,0,0 };
    direction = glm::rotate(direction, glm::radians(x_polar), glm::vec3{0,1,0});
    direction = glm::rotate(direction, glm::radians(y_polar), glm::cross(glm::vec3{ direction }, glm::vec3{ 0,1,0 }));

    direction *= radius;
    cam = Camera{ direction, -direction, EY, 60.f, static_cast<float>(windowWidth_) / windowHeight_, 0.1f, 100 };

    ImGui::PopItemWidth();
  }
  ImGui::End();
  if (ImGui::Begin("World Lighting"))
  {
    ImGui::PushItemWidth(400);
    glm::vec3 ambientColor = LightSystem::GetAmbientColor();
    glm::vec3 fogColor = LightSystem::GetFogColor();
    std::pair<float,float> range = LightSystem::GetFogRange();
    glm::vec3 attenuation = LightSystem::GetLightAttenuation();

    ImGui::ColorEdit3("Ambient Color", data(ambientColor));
    ImGui::ColorEdit3("Fog Color", data(fogColor));

    ImGui::PushItemWidth(198);
    ImGui::DragFloat("##minFogRange", &range.first, 0.01f, 0.f, range.second);
    ImGui::SameLine(210); ImGui::DragFloat("Fog Range", &range.second, 0.01f, range.first, 100.f);
    ImGui::PopItemWidth();

    ImGui::DragFloat3("Light Attenuation", data(attenuation), 0.01f, 0.f, 2.f);

    LightSystem::SetAmbientColor(ambientColor);
    LightSystem::SetFog(fogColor, range.first, range.second);
    LightSystem::SetLightAttenuation(attenuation);

    ImGui::Separator();

    const auto& lightObjects = LightObjects();
    int prevLightCount = activeLightCount_;
    if(ImGui::DragInt("LightCount", &activeLightCount_, 0.1f, 0, LightSystem::lightCount))
    {
      for (int i = std::min(activeLightCount_, prevLightCount);
               i < std::max(activeLightCount_, prevLightCount); ++i)
      {
        lightObjects[i]->SetActive(i < activeLightCount_);
      }
    }

    float distance = lightObjects[0]->GetComponent<TransformComponent>()->GetPivot().x;
    if (ImGui::SliderFloat("Global Light Distance", &distance, 1.5, 10))
    {
      for (GameObject* lightObject : lightObjects)
      {
        auto* lightTransform = lightObject->GetComponent<TransformComponent>();
        lightTransform->SetPivot({ distance, 0, 0 });
      }
    }

    ImGui::PopItemWidth();
  }
  ImGui::End();
  if(ImGui::Begin("Objects"))
  {
    for (GameObject* o : objects_)
    {
      o->ImGuiEditor();
    }
  }
  ImGui::End();

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("Power Plant"))
    {
      ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
      for(auto *obj : PowerPlantGroupObjects())
      {
        bool checked = obj->IsActive();
        bool loaded = obj->HasComponent<ChildListComponent>();
        const ImGuiStyle& style = ImGui::GetStyle();
        if(!loaded) ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
        ImGui::MenuItem(obj->Name().c_str(), nullptr, &checked);
        if (!loaded) ImGui::PopStyleColor();
        obj->SetActive(checked);
      }
      ImGui::PopItemFlag();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  cam.UpdateGPUCamera();
  VertexNormalRender::setCamera(cam);
  FaceNormalRender::setCamera(cam);

  LightSystem::SetEyePos(cam.eye());
  LightSystem::Update();

  MaterialSystem::Update();

  int i = 0;
  auto activeLights = ActiveLights();
  for(auto* light : activeLights)
  {
    auto* transform = light->GetComponent<TransformComponent>();
    transform->SetRotationAngle(-angleOfRotation + (360.f / activeLights.size() * static_cast<float>(i)));
    ++i;
  }

  SolidRender::clear(glm::vec4(LightSystem::GetFogColor(), 1));
#if DEFERRED
  GBuffer::Bind();
  SolidRender::clear(glm::vec4(std::numeric_limits<float>::max()));
#endif
  for (GameObject* o : objects_) {
    o->PreRender();
    o->Render();
  }

#if DEFERRED
  GBuffer::UnBind();
  GBuffer::RenderFSQ();
#endif

  for (GameObject* o : objects_) {
    o->PreRender();
    o->DebugRender();
  }

  GBuffer::ImguiEditor();

  return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int MainScene::postRender()
{
  angleOfRotation += 0.5f;

  return 0;
}

void MainScene::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_REPEAT || action == GLFW_PRESS)
  {
    switch (key)
    {
    case GLFW_KEY_W:
      y_polar += 3.f;
      y_polar = glm::clamp(y_polar, -89.9f, 89.9f);
      break;
    case GLFW_KEY_S:
      y_polar -= 3.f;
      y_polar = glm::clamp(y_polar, -89.9f, 89.9f);
      break;
    case GLFW_KEY_A:
      x_polar -= 3.f;
      if (x_polar < -180)
        x_polar += 360.f;
      break;
    case GLFW_KEY_D:
      x_polar += 3.f;
      if (x_polar > 180)
        x_polar -= 360.f;
      break;
    case GLFW_KEY_E:
      radius -= .2f;
      radius = glm::clamp(radius, 0.f, std::numeric_limits<float>::max());
      break;
    case GLFW_KEY_Q:
      radius += .2f;
      break;
    }
  }
}