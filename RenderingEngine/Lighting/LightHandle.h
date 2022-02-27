#pragma once
#include "Utilities/Utilities.h"
#include "LightSystem.h"


class LightHandle
{
public:
  friend class LightSystem;
  static LightHandle null();
  using LightType = Light::LightType;

  LightHandle();
  LightHandle(size_t index);
  LightHandle& operator=(const LightHandle other);

  [[nodiscard]] const glm::vec3 &GetPosition() const;
  [[nodiscard]] glm::vec3 GetDirection() const;

  [[nodiscard]] const glm::vec3 &GetDiffuseColor() const;
  [[nodiscard]] const glm::vec3 &GetSpecularColor() const;
  [[nodiscard]] const glm::vec3 &GetAmbientColor() const;

  [[nodiscard]] Light::LightType GetType() const;
  [[nodiscard]] std::pair<float,float> GetSpotlightAngles() const;
  [[nodiscard]] float GetSpotlightFalloff() const;

  [[nodiscard]] bool IsActive() const;
  [[nodiscard]] size_t GetIndex() const;

  void SetPosition(const glm::vec3& pos);
  void SetDirection(const glm::vec3& dir);

  void SetDiffuseColor(const glm::vec3& color);
  void SetSpecularColor(const glm::vec3& color);
  void SetAmbientColor(const glm::vec3& color);

  void SetType(LightType type);
  void SetSpotlightAngles(float inner, float outer);
  void SetSpotlightAngles(const std::pair<float,float> &angles);
  void SetSpotlightFalloff(float falloff);

  void SetActive(bool active);
  void Activate();
  void Deactivate();

  void SetIndex(size_t index);
  void Clone(const LightHandle other);

private:
  [[nodiscard]] Light& getData() const;

  size_t index_;
};