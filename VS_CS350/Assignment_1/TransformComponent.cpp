#include "TransformComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "imgui.h"
#include "VertexGlobalSystem.h"

void TransformComponent::PreRender()
{
  VertexGlobalSystem::SetModelToWorld(GetModel());
}

bool LockedDragFloat3(const char* label, glm::vec3 &v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
{
  glm::vec3 change = v;
  if(ImGui::DragFloat3(label, data(change), v_speed, v_min, v_max, format, flags))
  {
    float diff = 0;
    for (int i = 0; i < 3; ++i)
      diff += change[i] - v[i];
    for (int i = 0; i < 3; ++i)
      v[i] += diff;
    return true;
  }
  return false;
}

void TransformComponent::ImGuiEditor()
{
  if(lockTranslation_)
    LockedDragFloat3("Translation", translation_, 0.001f);
  else
    ImGui::DragFloat3("Translation", data(translation_), 0.001f);
  ImGui::SameLine();
  ImGui::Checkbox("Lock Translation", &lockTranslation_);


  if (lockScale_)
    LockedDragFloat3("Scale", scale_, 0.001f);
  else
    ImGui::DragFloat3("Scale", data(scale_), 0.001f);
  ImGui::SameLine();
  ImGui::Checkbox("Lock Scale", &lockScale_);

  if (lockPivot_)
    LockedDragFloat3("Pivot", pivot_, 0.001f);
  else
    ImGui::DragFloat3("Pivot", data(pivot_), 0.001f);
  ImGui::SameLine();
  ImGui::Checkbox("Lock Pivot", &lockPivot_);

  ImGui::DragFloat3("Rotation Normal", data(rotation_.normal_), 0.001f, -1.f, 1.f);
  ImGui::DragFloat("Rotation Angle", &rotation_.angle_);
}

std::string TransformComponent::Name()
{
  return "Transform Component";
}

TransformComponent::TransformComponent(const glm::vec3& translation, const glm::vec3& pivot, 
  const Rotation& rotation, const glm::vec3& scale):
    translation_(translation),
    pivot_(pivot),
    rotation_(rotation),
    scale_(scale)
{
}

TransformComponent::TransformComponent(const glm::vec3& translation, const glm::vec3& pivot,
  const Rotation& rotation, float scale) :
  translation_(translation),
  pivot_(pivot),
  rotation_(rotation),
  scale_(scale)
{
}

void TransformComponent::SetTranslation(const glm::vec3& translation)
{
  translation_ = translation;
}

void TransformComponent::SetPivot(const glm::vec3& pivot)
{
  pivot_ = pivot;
}

void TransformComponent::SetRotation(const Rotation& rotation)
{
  rotation_ = rotation;
}

void TransformComponent::SetRotationAngle(float rotation)
{
  rotation_.angle_ = rotation;
}

void TransformComponent::SetScale(const glm::vec3& scale)
{
  scale_ = scale;
}

void TransformComponent::SetScale(float scale)
{
  scale_ = glm::vec3(scale);
}

const glm::vec3& TransformComponent::GetTranslation() const
{
  return translation_;
}

const glm::vec3& TransformComponent::GetPivot() const
{
  return pivot_;
}

const TransformComponent::Rotation& TransformComponent::GetRotation() const
{
  return rotation_;
}

float TransformComponent::GetRotationAngle() const
{
  return rotation_.angle_;
}

const glm::vec3& TransformComponent::GetScale() const
{
  return scale_;
}

glm::vec3 TransformComponent::GetPosition() const
{
  return translation_ + rotate(pivot_, glm::radians(rotation_.angle_), rotation_.normal_);
}

glm::mat4 TransformComponent::GetModel() const
{
  //TODO: USE DIRTY VAR TO MAKE THIS FASTER
  glm::mat4 out{1};
  out = translate(out, translation_);
  out = rotate(out, glm::radians(rotation_.angle_), rotation_.normal_);
  out = translate(out, pivot_);
  out = scale(out, scale_);
  return out;
}
