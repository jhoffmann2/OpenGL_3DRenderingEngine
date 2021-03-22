#pragma once
#include <glm/vec3.hpp>
#include "ntg/simplex.h"

namespace ntg
{
  using namespace glm;
  template<length_t vcl = 3, typename vct = float>
  class hit
  {
  public:
    vec<vcl,vct> point;
    float distance;
    simplex<vcl,vcl,vct> triangle;
  };
}
