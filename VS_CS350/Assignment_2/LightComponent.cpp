#include "LightComponent.h"

#include <vector>

#include "imgui.h"
#include "GameObject.h"
#include "imgui_internal.h"
#include "TransformComponent.h"


LightComponent::LightComponent() : LightHandle()
{
}

LightComponent::LightComponent(size_t index) : LightHandle(index)
{
}

void LightComponent::PreRender()
{
  glm::vec3 pos = Parent().GetComponent<TransformComponent>()->GetPosition();
  SetPosition(pos);
  if(pointToTarget_)
  {
    glm::vec3 dir = target_ - pos;
    SetDirection(dir);
  }
}

void LightComponent::OnObjectActivated()
{
  Activate();
}

void LightComponent::OnObjectDeactivated()
{
  Deactivate();
}

void LightComponent::ImGuiEditor()
{
  int lightIndex = GetIndex();
  size_t itemCount = 0;
  static char buffer[16][16]{{0}};
  static char* items[16]{ nullptr };
  static bool items_setup = false;
  char(*itemsitr)[16] = buffer;
  if (!items_setup) // grr... dumb thing i have to do to make imgui happy
  {
    for (auto& item : items)
    {
      item = *itemsitr;
      ++itemsitr;
    }
    items_setup = true;
  }

  itemsitr = buffer;
  std::vector<LightHandle> unusedLights;

  sprintf_s(*itemsitr, 16, "LIGHT: %i", lightIndex);
  unusedLights.push_back(*this);
  ++itemsitr;
  ++itemCount;

  for(int i = 0; i < 16; ++i)
  {
    if (lightIndex == i)
      continue;
    LightHandle light(i);
    if(!light.IsActive())
    {
      sprintf_s(*itemsitr, 16, "LIGHT: %i", i);
      unusedLights.push_back(light);
      ++itemsitr;
      ++itemCount;
    }
  }

  int item_current = 0;
  if(ImGui::Combo("Light ID", &item_current, items, itemCount))
  {
    bool temp = IsActive();
    Deactivate();
    SetIndex(unusedLights[item_current].GetIndex());
    SetActive(temp);
  }

  bool active = IsActive();
  ImGui::Checkbox("Active", &active);
  if (active)
  {
    ImGui::Indent(20);
    ImGui::PushItemWidth(400);

    LightType type = GetType();
    const char* lightTypeNames[3] = { "POINT", "DIRECTIONAL", "SPOTLIGHT" };
    ImGui::Combo("Type", reinterpret_cast<int *>(&type), lightTypeNames, 3);

    ImGui::Separator();

    glm::vec3 diffuseColor = GetDiffuseColor();
    glm::vec3 specularColor = GetSpecularColor();
    glm::vec3 ambientColor = GetAmbientColor();
    glm::vec3 direction = GetDirection();
    std::pair<float,float> spotlightAngles = GetSpotlightAngles();
    float spotlightFalloff = GetSpotlightFalloff();

    ImGui::ColorEdit3("Diffuse Color", data(diffuseColor));
    ImGui::ColorEdit3("Specular Color", data(specularColor));
    ImGui::ColorEdit3("Ambient Color", data(ambientColor));

    ImGui::Separator();

    unless(type == LightType::POINT)
    {
      ImGui::Checkbox("Point At", &pointToTarget_);
      if(!pointToTarget_)
      {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      }
      ImGui::SameLine();
      ImGui::DragFloat3("##target", data(target_), 0.001f);
      if (!pointToTarget_)
      {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
      }
      if (pointToTarget_)
      {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      }
      ImGui::DragFloat3("Direction", data(direction), 0.001f, -1.f, 1.f);
      if (pointToTarget_)
      {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
      }
    }

    ImGui::Separator();

    if(type == LightType::SPOTLIGHT)
    {
      ImGui::PushItemWidth(198);
      ImGui::DragFloat("##minSpotlightAngle", &spotlightAngles.first, .05f, 0.f, spotlightAngles.second);
      ImGui::SameLine(270); ImGui::DragFloat("Spotlight Angles", &spotlightAngles.second, .05f, spotlightAngles.first, 180.f);
      ImGui::PopItemWidth();

      ImGui::DragFloat("Spotlight Falloff", &spotlightFalloff, 0.01f, 0.001f, std::numeric_limits<float>::max());
    }

    SetType(type);
    SetDiffuseColor(diffuseColor);
    SetSpecularColor(specularColor);
    SetAmbientColor(ambientColor);
    SetDirection(direction);
    SetSpotlightAngles(spotlightAngles);
    SetSpotlightFalloff(spotlightFalloff);

    ImGui::PopItemWidth();
    ImGui::Unindent(20);
  }
  SetActive(active);
}

std::string LightComponent::Name()
{
  return "Light Component";
}
