#include "MaterialComponent.h"
#include <imgui.h>

void MaterialComponent::PreRender()
{
  MaterialSystem::SetCurrentMaterialIndex(GetIndex());
}

MaterialComponent::MaterialComponent(size_t index) : MaterialHandle(index)
{
}

MaterialComponent::MaterialComponent(const MaterialHandle &handle) : MaterialHandle(handle)
{
}

void MaterialComponent::ImGuiEditor()
{
  int index = (int)GetIndex();
  ImGui::InputInt("Material ID:", &index);
  index = glm::clamp(index, 0, static_cast<int>(MaterialSystem::materialCount));
  SetIndex(index);

  glm::vec3 ke = GetEmissiveColor();
  glm::vec3 ka = GetAmbientColor();
  glm::vec3 kd = GetDiffuseColor();
  glm::vec3 ks = GetSpecularColor();
  float ns = GetSpecularExponent();
  TextureMode textureMode = GetTextureMode();

  ImGui::ColorEdit3("Emmisive Color", data(ke));
  ImGui::ColorEdit3("Ambient Color", data(ka));
  ImGui::ColorEdit3("Diffuse Color", data(kd));
  ImGui::ColorEdit3("Specular Color", data(ks));
  ImGui::SliderFloat("specular Exponent ", &ns, 0.001f, 40.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

  ImGui::Combo(
    "Texture UV Mapping",
    reinterpret_cast<int *>(&textureMode),
    "CPU\0"
    "GPU_PLANAR\0"
    "GPU_CYLINDRICAL\0"
    "GPU_SPHYRICAL\0"
  );

  SetEmissiveColor(ke);
  SetAmbientColor(ka);
  SetDiffuseColor(kd);
  SetSpecularColor(ks);
  SetSpecularExponent(ns);
  SetTextureMode(textureMode);
}

std::string MaterialComponent::Name()
{
  return "Material Component";
}
