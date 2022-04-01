#pragma once
#include "Utilities/Utilities.h"
#include <GL/glew.h>

struct Material
{
  enum TextureMode
  {
    TM_CPU,

    TM_GPU_PLANAR,
    TM_GPU_CYLINDRICAL,
    TM_GPU_SPHYRICAL
  };

  AlignData<16, glm::vec3> EmissiveColor_;
  AlignData<16, glm::vec3> ka_;
  AlignData<16, glm::vec3> kd_;
  AlignData<12, glm::vec3> ks_;
  AlignData<4, float> ns_ = 200.f;
  AlignData<4, TextureMode> textureMode_;
  AlignData<4, float> specularStrength = 1.f;
};

