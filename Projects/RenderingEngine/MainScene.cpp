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
#include <numeric>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/easing.hpp>

#include "Hierarchy/ParentChildComponent.h"
#include "Lighting/LightComponent.h"
#include "Lighting/LightSystem.h"
#include "Materials/MaterialComponent.h"
#include "Materials/MaterialSystem.h"
#include "Mesh/Mesh.h"
#include "Rendering/DebugDraw.h"
#include "Rendering/FaceNormalRender.h"
#include "Rendering/GBuffer.h"
#include "Rendering/RenderingComponent.h"
#include "Rendering/ShaderGlobalSystem.h"
#include "Rendering/VertexNormalRender.h"
#include "SpacialPartitioning/BspTree.h"
#include "SpacialPartitioning/Octree.h"
#include "SpacialPartitioning/SpacialTree.h"
#include "Transform/TransformComponent.h"
#include "Utilities/Utilities.h"
#include "Volumes/BoxVolumeComponent.h"
#include "glm/gtc/random.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "ntg/bounds.inl"
#include "ntg/hit.inl"
#include "ntg/hyperplane.inl"
#include "ntg/ray.inl"
#include "ntg/simplex.inl"

#include <filesystem>

#define DEFERRED true
#define LOG_ACTIVE true


const glm::vec3 O = {0, 0, 0},
                EX = {1, 0, 0},
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
static const glm::vec3 cam_pos{0.f, .75f, 3.75f};

MainScene::MainScene(int windowWidth, int windowHeight) :
  Scene(windowWidth, windowHeight),
  angleOfRotation(0.0f),
  viewportWidth_(windowWidth),
  viewportHeight_(windowHeight),
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

ntg::bounds3 MainScene::CalculateActivePowerPlantBounds() const
{
  ntg::bounds3 out;
  for (const auto* pc : PowerPlantGroup())
  {
    GameObject* obj = pc->GetGameObject();
    if (obj->IsActive())
    {
      auto* volumeComponent = obj->GetComponent<BoxVolumeComponent>();
      out = out.grow(volumeComponent->volume_);
    }
  }
  return out;
}

void MainScene::UpdateActivePowerPlants()
{
  constexpr size_t lerp_time = 60; // lerp over 60 frames
  static size_t cur_lerp = lerp_time + 1;


  auto* transform = objects_[POWER_PLANT]->GetComponent<TransformComponent>();
  auto* octree = SpacialTreeHierarchy::GetTree<Octree>(objects_[POWER_PLANT]);

  for (ParentChildComponent* pc : PowerPlantGroup())
  {
    GameObject* gameObject = pc->GetGameObject();
    if (gameObject->IsActive() && pc->Children().empty())
    {
      std::vector<Mesh> meshes;
      std::vector<std::string> names;

      ImportMeshList(
        "../../Common/models/Powerplant/",
        gameObject->Name() + ".txt",
        meshes,
        names
      );

      std::cout << "centering and normalizing meshes" << std::endl;
      MeshTransform(meshes, powerPlantTransformation_);

      pc->AddChildrenFromMeshes(meshes, names);
      cur_lerp = lerp_time + 1;
//      for (size_t i = 0; i < meshes.size(); ++i)
//      {
//        std::cout << "adding [" << names[i] << "] to octree" << std::endl;
//        octree->Add(meshes[i]);
//      }
//
//      SpacialTreeHierarchy::SetTree(
//        gameObject,
//        new BspTree(meshes)
//      );
      break; // only load one per frame
    }
  }

  ntg::bounds3 cur_bounds = CalculateActivePowerPlantBounds();
  if (cur_bounds != activePowerPlantBounds)
  {
    cur_lerp = lerp_time + 1;
    activePowerPlantBounds = cur_bounds;
    }

  // update the pivot so powerplant is centered;
  static glm::vec3 destination_pivot;
  static glm::vec3 start_pivot;
  if (cur_lerp > lerp_time)
  {
    cur_lerp = 1;

    ntg::bounds3 bounds = transform->GetModelToWorld() * cur_bounds;
    ShaderGlobalSystem::SetSceneBounds(bounds);

    glm::vec3 center = bounds.center();
    if (bounds.valid())
      center.y = bounds.min.y + 0.25f * bounds.size().y; // favor looking toward the ground

    start_pivot = transform->GetPivot();
    destination_pivot = start_pivot - center;
  }

  float t = glm::sineEaseInOut(static_cast<float>(cur_lerp) / lerp_time);
  transform->SetPivot(mix(start_pivot, destination_pivot, t));
  ++cur_lerp;
}

const std::unordered_set<ParentChildComponent*>& MainScene::PowerPlantGroup() const
{
  return objects_[POWER_PLANT]->GetComponent<ParentChildComponent>()->Children();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void MainScene::CleanUp()
{
  for (GameObject* obj : objects_)
  {
    delete obj;
  }
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int MainScene::Init()
{
  GBuffer::Init(viewportWidth_, viewportHeight_);

  ShaderGlobalSystem::Init();

  const auto sphere_meshData(generateSphereMesh(30));

  const int sphere_mesh = SolidRender::loadMesh(sphere_meshData);
  VertexNormalRender::loadMesh(sphere_meshData);
  FaceNormalRender::loadMesh(sphere_meshData);

  std::vector<ntg::bounds3> powerPlantBounds;
  if (std::filesystem::exists("../../Common/models/Powerplant/bounds.txt"))
  {
    std::ifstream boundsFile("../../Common/models/Powerplant/bounds.txt");
    while (!boundsFile.eof())
    {
      powerPlantBounds.emplace_back();
      boundsFile >> powerPlantBounds.back();
    }
    powerPlantBounds.pop_back();
  }
  else
  {
    std::cout << "calculating power plant size" << std::endl;
    for (size_t i = 1; i <= powerPlantGroupCount; ++i)
    {
      powerPlantBounds.emplace_back();
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
      powerPlantBounds.back() = powerPlantBounds.back().grow(categoryBounds);
    }

    std::ofstream boundsFile("../../Common/models/Powerplant/bounds.txt");
    ntg::bounds3 fullPlantBounds;
    for (const auto& b : powerPlantBounds)
    {
      fullPlantBounds = fullPlantBounds.grow(b);
      boundsFile << b << std::endl;
    }
    powerPlantBounds.emplace_back(fullPlantBounds);
    boundsFile << fullPlantBounds << std::endl;
  }

  objects_.emplace_back(new GameObject("Power Plant"));

  TransformComponent* transform = new TransformComponent(
    {0, 0, 0},
    {0, 0, 0},
    {EY, 0},
    40.f
  );
  objects_.back()->AddComponent(transform);


  MaterialHandle material(0);
  material.SetEmissiveColor(glm::vec3{0});
  material.SetAmbientColor(glm::vec3{.0742f});
  material.SetDiffuseColor(glm::vec3{.6392f, .5412f, .4118f});
  material.SetSpecularColor(glm::vec3{0.3176f});
  material.SetSpecularExponent(20);
  objects_.back()->AddComponent(new MaterialComponent(material));
  auto* pc = new ParentChildComponent();

  powerPlantTransformation_ = CenterMeshTransform(powerPlantBounds.back());
  SpacialTreeHierarchy::SetTree(
    objects_.back(),
    new Octree(powerPlantTransformation_ * powerPlantBounds.back())
  );

  for (size_t i = 0; i < powerPlantGroupCount; ++i)
  {
    const std::string name = "Section" + std::to_string(i + 1);
    auto* section = new GameObject(name);
    section->SetActive(i == 9); // start w/ only section 10 loaded
    auto* section_pc = new ParentChildComponent(pc);
    section->AddComponent(section_pc);
    section->AddComponent(new BoxVolumeComponent(powerPlantTransformation_ * powerPlantBounds[i]));
    pc->AddChild(section_pc);
  }
  objects_.back()->AddComponent(pc);

  {
    objects_.emplace_back(new GameObject("Lights"));
    auto* pc = new ParentChildComponent();
    objects_.back()->AddComponent(pc);

    for (size_t i = 0; i < LightSystem::lightCount; ++i)
    {
      std::stringstream name;
      name << "Light #" << i;

      auto* lightObj = new GameObject(name.str());
      lightObj->AddComponent(
        new TransformComponent(
          {0, 0, 0},
          {3.7, 0, 0},
          {EY, 0},
          3.f
        )
      );

      const float hue = glm::linearRand(0.f, 360.f);
      MaterialHandle light_material(MaterialSystem::materialCount - LightSystem::lightCount + i);
      light_material.SetEmissiveColor(rgbColor(glm::vec3{hue, .2f, 1.f}));
      light_material.SetDiffuseColor(rgbColor(glm::vec3{hue, .25f, 1.f}));
      light_material.SetSpecularColor(rgbColor(glm::vec3{hue, .1f, 1.f}));
      light_material.SetAmbientColor(rgbColor(glm::vec3{hue, .25f, 0.2f}));
      light_material.SetSpecularExponent(20);

      auto* rendering = new RenderingComponent(sphere_mesh, SolidRender::LOCAL_LIGHT);
      lightObj->AddComponent(rendering);

      lightObj->AddComponent(new MaterialComponent(light_material));

      auto* light_pc = new ParentChildComponent(pc);
      lightObj->AddComponent(light_pc);

      lightObj->SetActive(false);

      pc->AddChild(light_pc);
      lights_.emplace_back(lightObj);
    }
  }
  for (int i = 0; i < activeLightCount_; ++i)
    lights_[i]->Activate();

  VertexNormalRender::setCamera(cam);
  FaceNormalRender::setCamera(cam);

  LightSystem::Init();
  LightSystem::SetAmbientColor({.4078f, .6627f, 1.f});
  LightSystem::SetLightAttenuation({0.f, .2f, .02f});
  LightSystem::SetFog({.6039f, .6157f, .6549f}, 0, 40);

  MaterialSystem::Init();

  return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int MainScene::Render()
{
  UpdateActivePowerPlants();

  if (ImGui::Begin("Camera"))
  {
    ImGui::PushItemWidth(400);

    ImGui::SliderFloat("X rotation", &camPolarX, -180, 180, "%.1f degrees");
    ImGui::SliderFloat("Y rotation", &camPolarY, -89.9f, 89.9f, "%.1f degrees");
    ImGui::DragFloat("Radius", &camRadius, 0.002f);
    ImGui::SliderFloat("FOV", &camFOV, 0, 180, "%.1f degrees");

    glm::vec3 direction{1, 0, 0};
    direction = glm::rotate(direction, glm::radians(camPolarX), glm::vec3{0, 1, 0});
    direction = glm::rotate(direction, glm::radians(camPolarY), glm::cross(glm::vec3{direction}, glm::vec3{0, 1, 0}));

    direction *= camRadius;
    cam = Camera{direction, -direction, EY, camFOV, static_cast<float>(viewportWidth_) / viewportHeight_, 0.1f, 100};

    ImGui::PopItemWidth();
  }
  ImGui::End();
  if (ImGui::Begin("World Lighting"))
  {
    ImGui::PushItemWidth(400);
    glm::vec3 ambientColor = LightSystem::GetAmbientColor();
    glm::vec3 fogColor = LightSystem::GetFogColor();
    std::pair<float, float> range = LightSystem::GetFogRange();
    glm::vec3 attenuation = LightSystem::GetLightAttenuation();

    ImGui::ColorEdit3("Ambient Color", data(ambientColor));
    ImGui::ColorEdit3("Fog Color", data(fogColor));

    ImGui::PushItemWidth(198);
    ImGui::DragFloat("##minFogRange", &range.first, 0.01f, 0.f, range.second);
    ImGui::SameLine(210);
    ImGui::DragFloat("Fog Range", &range.second, 0.01f, range.first, 100.f);
    ImGui::PopItemWidth();

    ImGui::DragFloat3("Light Attenuation", data(attenuation), 0.01f, 0.f, 2.f);

    LightSystem::SetAmbientColor(ambientColor);
    LightSystem::SetFog(fogColor, range.first, range.second);
    LightSystem::SetLightAttenuation(attenuation);

    ImGui::Separator();

    int prevLightCount = activeLightCount_;
    if (ImGui::DragInt("LightCount", &activeLightCount_, 0.1f, 0, LightSystem::lightCount))
    {
      for (int i = std::min(activeLightCount_, prevLightCount);
           i < std::max(activeLightCount_, prevLightCount); ++i)
      {
        lights_[i]->SetActive(i < activeLightCount_);
      }
    }

    float distance = lights_[0]->GetComponent<TransformComponent>()->GetPivot().x;
    if (ImGui::SliderFloat("Global Light Distance", &distance, 1.5, 10))
    {
      for (GameObject* lightObject : lights_)
      {
        auto* lightTransform = lightObject->GetComponent<TransformComponent>();
        lightTransform->SetPivot({distance, 0, 0});
      }
    }

    ImGui::PopItemWidth();
  }
  ImGui::End();
  if (ImGui::Begin("Objects"))
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
      for (auto* pc : PowerPlantGroup())
      {
        GameObject* obj = pc->GetGameObject();
        bool checked = obj->IsActive();
        bool loaded = !obj->GetComponent<ParentChildComponent>()->Children().empty();
        const ImGuiStyle& style = ImGui::GetStyle();
        if (!loaded) ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
        ImGui::MenuItem(obj->Name().c_str(), nullptr, &checked);
        if (!loaded) ImGui::PopStyleColor();
        obj->SetActive(checked);
      }
      ImGui::PopItemFlag();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("LightConversion"))
    {
      ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
      for(size_t i = 0; i < lights_.size(); ++i)
      {
        GameObject* light = lights_[i];
        if(!light->IsActive())
          continue;

        int isGlobal = light->HasComponent<LightComponent>();
        ImGui::Text("%s", light->Name().c_str()); ImGui::SameLine();
        ImGui::RadioButton(("Global##" + light->Name()).c_str(), &isGlobal, 1); ImGui::SameLine();
        ImGui::RadioButton(("Local##" + light->Name()).c_str(), &isGlobal, 0);
        if(isGlobal && !light->HasComponent<LightComponent>())
        {
          RenderingComponent *rendering = light->GetComponent<RenderingComponent>();
          TransformComponent *transform = light->GetComponent<TransformComponent>();
          MaterialComponent *material = light->GetComponent<MaterialComponent>();
          rendering->SetShader(SolidRender::FLAT_EMISSION);
          transform->SetScale(transform->GetScale() * 0.1f);

          auto* lightComponent = new LightComponent(i);
          lightComponent->SetDiffuseColor(material->GetDiffuseColor());
          lightComponent->SetSpecularColor(material->GetSpecularColor());
          lightComponent->SetAmbientColor(material->GetAmbientColor());
          lightComponent->SetType(Light::POINT);
          lightComponent->SetActive(true);
          light->AddComponent(lightComponent);
        }
        else if(!isGlobal && light->HasComponent<LightComponent>())
        {
          RenderingComponent *rendering = light->GetComponent<RenderingComponent>();
          TransformComponent *transform = light->GetComponent<TransformComponent>();
          rendering->SetShader(SolidRender::LOCAL_LIGHT);
          transform->SetScale(transform->GetScale() * 10.f);
          light->GetComponent<LightComponent>()->SetActive(false);
          light->RemoveComponent<LightComponent>();
        }
      }

      ImGui::PopItemFlag();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  LightSystem::SetEyePos(cam.eye());
  LightSystem::Update();

  MaterialSystem::Update();


  for (int i = 0; i < activeLightCount_; ++i)
  {
    auto* transform = lights_[i]->GetComponent<TransformComponent>();
    transform->SetRotationAngle(-angleOfRotation + (360.f / activeLightCount_ * static_cast<float>(i)));
  }

  SolidRender::clear(glm::vec4(LightSystem::GetFogColor(), 1));


#if DEFERRED
  GBuffer::Bind();

  glViewport(0, 0, viewportHeight_, viewportHeight_);
  SolidRender::clear(glm::vec4(std::numeric_limits<float>::max()));
  for(GameObject* light : lights_)
  {
    if(light->HasComponent<LightComponent>())
    {
      light->GetComponent<LightComponent>()->RenderShadowMap(objects_);
      break;
    }
  }
#endif

  cam.UpdateGPUCamera();
  VertexNormalRender::setCamera(cam);
  FaceNormalRender::setCamera(cam);

#if DEFERRED
  glViewport(0, 0, viewportWidth_, viewportHeight_);
  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT);

  for (GameObject* o : objects_)
  {
    o->PreRender();
    o->Render();
  }

  GBuffer::UnBind();
  GBuffer::RenderFSQ();
#endif

  for (GameObject* o : objects_)
  {
    o->PreRender();
    o->ForwardRender();
  }


  glm::mat4 NDCToWorld = inverse(ShaderGlobalSystem::GetCamToNDC() * transpose(inverse(ShaderGlobalSystem::GetWorldToCam())));
  ntg::ray3 r = {cam.eye(), NDCToWorld * mouse_ndc};

  ntg::hit3 hit;
  if (SpacialTreeHierarchy::Raycast(objects_[POWER_PLANT], r, hit))
  {
    DebugDraw::SetColor({1.f, 1.f, 1.f, 1.f});
    ShaderGlobalSystem::SetModelToWorld(glm::mat4{1.f});
    DebugDraw::DrawTriangleList({hit.triangle}, GL_FILL);
    DebugDraw::SetColor({0.f, 1.f, 0.f, 1.f});
    DebugDraw::DrawLineList({{hit.point, hit.point + hit.normal() * .5f}});
  }

  SpacialTreeHierarchy::ImguiDraw(r);

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
  if (action == GLFW_REPEAT || action == GLFW_PRESS)
  {
    switch (key)
    {
    case GLFW_KEY_W:
      camPolarY += 3.f;
      camPolarY = glm::clamp(camPolarY, -89.9f, 89.9f);
      break;
    case GLFW_KEY_S:
      camPolarY -= 3.f;
      camPolarY = glm::clamp(camPolarY, -89.9f, 89.9f);
      break;
    case GLFW_KEY_A:
      camPolarX -= 3.f;
      if (camPolarX < -180)
        camPolarX += 360.f;
      break;
    case GLFW_KEY_D:
      camPolarX += 3.f;
      if (camPolarX > 180)
        camPolarX -= 360.f;
      break;
    case GLFW_KEY_E:
      camRadius -= .2f;
      camRadius = glm::clamp(camRadius, 0.f, std::numeric_limits<float>::max());
      break;
    case GLFW_KEY_Q:
      camRadius += .2f;
      break;
    }
  }
}

void MainScene::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  int winWidth, winHeight;
  glfwGetWindowSize(window, &winWidth, &winHeight);
  ypos -= winHeight - viewportHeight_;
  mouse_ndc = {
    2.f * (xpos / viewportWidth_) - 1.f,
    -(2.f * (ypos / viewportHeight_) - 1.f),
    1.f,
    1.f
  };
}
