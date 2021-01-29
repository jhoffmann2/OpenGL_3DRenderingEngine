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
#include "Assignment1Scene.h"


#include <array>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <glm/gtx/color_space.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Affine.h"
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
#include "ParentComponent.h"
#include "VertexNormalRender.h"


#define LOG_ACTIVE true


const glm::vec4 O  = point (0, 0, 0),
                EX = vector(1, 0, 0),
                EY = vector(0, 1, 0),
                EZ = vector(0, 0, 1);

const glm::vec3 BLACK(0, 0, 0),
                WHITE(1, 1, 1),
                REDISH(1, 0.6f, 0.6f),
                GREENISH(0.6f, 1, 0.6f),
                BLUEISH(0.6f, 0.6f, 1);


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
Assignment1Scene::~Assignment1Scene()
{
  initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
static const glm::vec4 cam_pos{ 0.f,.75f,3.75f,1 };

Assignment1Scene::Assignment1Scene(int windowWidth, int windowHeight) :
  Scene(windowWidth, windowHeight),
  angleOfRotation(0.0f),
  windowWidth_(windowWidth),
  windowHeight_(windowHeight),
  cam(cam_pos, -cam_pos, EY, 60.f, static_cast<float>(windowWidth) / windowHeight, 0.01f, 100)
{
  initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void Assignment1Scene::initMembers()
{
  angleOfRotation = 0.0f;
  objects_.clear();
}

glm::bounds4 Assignment1Scene::ActivePowerPlantBounds()
{
  glm::bounds4 out;
  for(size_t i = 0; i < powerPlantGroupCount; ++i)
  {
    if(PowerPlantGroupObjects()[i]->IsActive())
    {
      out = out.grow(powerPlantBounds_[i]);
    }
  }
  return out;
}

void Assignment1Scene::UpdateActivePowerPlants()
{
  for (auto obj : PowerPlantGroupObjects())
  {
    if(obj->IsActive() && !obj->HasComponent<ParentComponent>())
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
        new ParentComponent(meshes, names)
      );
      break;
    }
  }
}

const std::vector<GameObject*>& Assignment1Scene::PowerPlantGroupObjects() const
{
  return objects_[0]->GetComponent<ParentComponent>()->Children();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void Assignment1Scene::CleanUp()
{
  for(GameObject *obj : objects_)
  {
    delete obj;
  }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int Assignment1Scene::Init()
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
    glm::bounds4 fullPlantBounds;
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
      { 7.911,6.078,0.856 },
      { EY,-203 },
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
  auto* pc = new ParentComponent();

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
    auto* obj = PowerPlantGroupObjects()[10];
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
      new ParentComponent(meshes, names)
    );
  }

  for(size_t i = 0; i < LightSystem::lightCount; ++i)
  {
    std::stringstream name;
    name << "Light #" << i;

    objects_.emplace_back(new GameObject(name.str()));
    objects_.back()->AddComponent(
      new TransformComponent(
        {0,0,0},
        {2,0,0},
        {EY,0},
        .02f
      )
    );

    auto* light = new LightComponent(i);
    const float hue = glm::linearRand(24.f,40.f);
    light->SetDiffuseColor(rgbColor(glm::vec3{hue, .25f, 1.f}));
    light->SetSpecularColor(rgbColor(glm::vec3{ hue, .1f, 1.f }));
    light->SetAmbientColor(rgbColor(glm::vec3{ hue, .25f, 0.2f }));
    light->SetType(Light::POINT);
    light->SetActive(true);
    objects_.back()->AddComponent(light);

    auto* rendering = new RenderingComponent(sphere_mesh, SolidRender::FLAT_EMISSION);
    objects_.back()->AddComponent(rendering);

    auto* material = new MaterialComponent(i + 1);
    material->SetEmissiveColor(rgbColor(glm::vec3{ hue, 0.0f, 1.f }));
    material->SetAmbientColor(glm::vec3{ 0.f });
    material->SetDiffuseColor(glm::vec3{ 0.f });
    material->SetSpecularColor(glm::vec3{ 0.f });
    material->SetSpecularExponent(20);
    objects_.back()->AddComponent(material);

    objects_.back()->SetActive(false);
  }

  lights_ = Range(objects_, 1, 4);
  for(auto *light : lights_)
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
int Assignment1Scene::Render()
{
  UpdateActivePowerPlants();

  if(ImGui::Begin("debug"))
  {
    glm::vec3 pivot = -(powerPlantTransformation_ * ActivePowerPlantBounds().center());
    auto* transform = objects_[0]->GetComponent<TransformComponent>();
    transform->SetPivot(pivot * transform->GetScale());
    ImGui::End();
  }


  if(ImGui::Begin("Camera"))
  {
    ImGui::PushItemWidth(400);

    ImGui::SliderFloat("X rotation", &x_polar, -180, 180, "%.1f degrees");
    ImGui::SliderFloat("Y rotation", &y_polar, -89.9f, 89.9f, "%.1f degrees");
    ImGui::DragFloat("Radius", &radius, 0.002f);
    glm::vec4 direction{ 1,0,0,0 };
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

    int lightCount = lights_.size();
    if(ImGui::DragInt("LightCount", &lightCount, 0.1f, 0, LightSystem::lightCount))
    {
      lightCount = glm::clamp(lightCount, 0, static_cast<int>(LightSystem::lightCount));
      if(static_cast<size_t>(lightCount) < lights_.size()) // light count reduced
      {
        for(size_t i = lightCount; i < lights_.size(); ++i)
        {
          lights_[i]->Deactivate();
        }
      }
      else // light count raised
      {
        float distance = lights_[0]->GetComponent<TransformComponent>()->GetPivot().x;
        for (size_t i = lights_.size(); i < static_cast<size_t>(lightCount); ++i)
        {
          lights_[i]->Activate();
          auto* lightTransform = lights_[i]->GetComponent<TransformComponent>();
          lightTransform->SetPivot({ distance, 0, 0 });
        }
      }
      lights_ = Range(objects_, 1, lightCount + 1);
    }

    float distance = lights_[0]->GetComponent<TransformComponent>()->GetPivot().x;
    if (ImGui::SliderFloat("Global Light Distance", &distance, 1.5, 10))
    {
      for (GameObject* lightObject : lights_)
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
      for(auto *obj : PowerPlantGroupObjects())
      {
        bool checked = obj->IsActive();
        ImGui::MenuItem(obj->Name().c_str(), nullptr, &checked);
        obj->SetActive(checked);
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Load Scenario"))
    {
      if (ImGui::MenuItem("Scenario 01"))
      {
        const float hue = glm::linearRand(0.f, 360.f);
        auto lightType = static_cast<Light::LightType>(glm::linearRand(0, 2));
        for(GameObject *light : lights_)
        {
          auto* lighting = light->GetComponent<LightComponent>();
          auto* material = light->GetComponent<MaterialComponent>();
          lighting->SetDiffuseColor(rgbColor(glm::vec3{ hue, 1.f, 1.f }));
          lighting->SetSpecularColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
          lighting->SetAmbientColor(rgbColor(glm::vec3{ hue, 0.2f, 0.2f }));
          lighting->SetType(lightType);
          lighting->SetActive(true);
          material->SetEmissiveColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
        }
      }
      if (ImGui::MenuItem("Scenario 02"))
      {
        auto lightType = static_cast<Light::LightType>(glm::linearRand(0, 2));
        for (GameObject* light : lights_)
        {
          const float hue = glm::linearRand(0.f, 360.f);

          auto* lighting = light->GetComponent<LightComponent>();
          auto* material = light->GetComponent<MaterialComponent>();
          lighting->SetDiffuseColor(rgbColor(glm::vec3{ hue, 1.f, 1.f }));
          lighting->SetSpecularColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
          lighting->SetAmbientColor(rgbColor(glm::vec3{ hue, 0.2f, 0.2f }));
          lighting->SetType(lightType);
          lighting->SetActive(true);
          material->SetEmissiveColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
        }
      }
      if (ImGui::MenuItem("Scenario 03"))
      {

        for (GameObject* light : lights_)
        {
          const float hue = glm::linearRand(0.f, 360.f);
          auto lightType = static_cast<Light::LightType>(glm::linearRand(0, 2));

          auto* lighting = light->GetComponent<LightComponent>();
          auto* material = light->GetComponent<MaterialComponent>();
          lighting->SetDiffuseColor(rgbColor(glm::vec3{ hue, 1.f, 1.f }));
          lighting->SetSpecularColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
          lighting->SetAmbientColor(rgbColor(glm::vec3{ hue, 0.2f, 0.2f }));
          lighting->SetType(lightType);
          lighting->SetActive(true);
          material->SetEmissiveColor(rgbColor(glm::vec3{ hue, 0.5f, 1.f }));
        }
      }
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

  /*{
    auto* transform = objects_[0]->GetComponent<TransformComponent>();
    transform->SetRotationAngle(angleOfRotation);
  }*/

  int i = 0;
  for(auto* light : lights_)
  {
    auto* transform = light->GetComponent<TransformComponent>();
    transform->SetRotationAngle(-angleOfRotation + (360.f / lights_.size() * static_cast<float>(i)));
    ++i;
  }

  SolidRender::clear(glm::vec4(LightSystem::GetFogColor(), 1));
  GBuffer::Bind();
  SolidRender::clear(glm::vec4(std::numeric_limits<float>::max()));

  for (GameObject* o : objects_) {
    o->PreRender();
    o->Render();
  }

  GBuffer::UnBind();

  GBuffer::RenderFSQ();
  for (GameObject* o : objects_) {
    o->PreRender();
    o->DebugRender();
  }

  return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int Assignment1Scene::postRender()
{
  angleOfRotation += 0.5f;

  return 0;
}

void Assignment1Scene::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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