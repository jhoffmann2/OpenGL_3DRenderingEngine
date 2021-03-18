#pragma once
#include "Utilities.h"


class Light
{
public:
  template<size_t alignment, typename T>
  friend class AlignData;
  friend class LightSystem;
  enum LightType
  {
    POINT,
    DIRECTIONAL,
    SPOTLIGHT
  };

  AlignData<16, glm::vec3> position_;
  AlignData<16, glm::vec3> direction_ = glm::vec3{1.f, 0.f, 0.f};
  AlignData<16, glm::vec3> diffuseColor_;
  AlignData<16, glm::vec3> specularColor_;
  AlignData<12, glm::vec3> ambientColor_;

  AlignData<4, LightType> type_ = POINT;
  AlignData<4, float> spotlightInnerAngle_ = 10.f;
  AlignData<4, float> spotlightOuterAngle_ = 15.f;
  AlignData<4, float> spotlightFalloff_ = 3.f;
  AlignData<4, bool> active_ = false;

private:
  Light() = default;
};