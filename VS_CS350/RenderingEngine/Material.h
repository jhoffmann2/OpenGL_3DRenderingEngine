#pragma once
#include "Utilities.h"
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
  AlignData<4, float> ns_;
  AlignData<4, TextureMode> textureMode_;
};

