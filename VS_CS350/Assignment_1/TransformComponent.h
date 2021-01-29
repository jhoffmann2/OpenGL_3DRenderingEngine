#pragma once
#include <glm/mat4x4.hpp>

#include "ObjectComponent.h"

class TransformComponent : public ObjectComponent
{
  struct Rotation;
public:
  void PreRender() override;
  void ImGuiEditor() override;
  std::string Name() override;

  TransformComponent(const glm::vec3& translation, const glm::vec3& pivot, const Rotation& rotation,
                     const glm::vec3& scale);
  TransformComponent(const glm::vec3& translation, const glm::vec3& pivot, const Rotation& rotation,
                     float scale);

  void SetTranslation(const glm::vec3& translation);
  void SetPivot(const glm::vec3& pivot);
  void SetRotation(const Rotation& rotation);
  void SetRotationAngle(float rotation);
  void SetScale(const glm::vec3& scale);
  void SetScale(float scale);
  
  [[nodiscard]]const glm::vec3 &GetTranslation() const;
  [[nodiscard]]const glm::vec3 &GetPivot() const;
  [[nodiscard]]const Rotation &GetRotation() const;
  [[nodiscard]]float GetRotationAngle() const;
  [[nodiscard]]const glm::vec3 &GetScale() const;

  [[nodiscard]]glm::vec3 GetPosition() const;
  [[nodiscard]]glm::mat4 GetModel() const;
private:

  glm::vec3 translation_;
  glm::vec3 pivot_;
  struct Rotation
  {
    glm::vec3 normal_;
    float angle_;
  } rotation_;
  glm::vec3 scale_;


  // used by imgui
  bool lockTranslation_ = false;
  bool lockScale_ = false;
  bool lockPivot_ = false;
};

