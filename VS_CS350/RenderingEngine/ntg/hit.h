#pragma once
#include <glm/vec3.hpp>
#include "simplex.h"

namespace ntg
{
  using namespace glm;
  template<length_t vcl = 3, typename vct = float>
  class hit
  {
  public:
    vec<vcl,vct> point{0};
    simplex<vcl,vcl,vct> triangle;
    vec<vcl, vct> normal();
  };

  template<length_t vcl = 3, typename vct = float>
  [[nodiscard]] hit<vcl, vct> operator*(const mat<vcl, vcl, vct>& transform, const hit<vcl, vct>& h);

  template<length_t vcl = 3, typename vct = float>
  [[nodiscard]] hit<vcl, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const hit<vcl, vct>& h);


  using hit2 = hit<2, float>;
  using hit3 = hit<3, float>;
  using hit4 = hit<4, float>;

  using ihit2 = hit<2, int>;
  using ihit3 = hit<3, int>;
  using ihit4 = hit<4, int>;

  using uhit2 = hit<2, unsigned>;
  using uhit3 = hit<3, unsigned>;
  using uhit4 = hit<4, unsigned>;

  using dhit2 = hit<2, double>;
  using dhit3 = hit<3, double>;
  using dhit4 = hit<4, double>;
}
