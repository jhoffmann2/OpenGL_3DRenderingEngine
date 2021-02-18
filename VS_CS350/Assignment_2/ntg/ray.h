#pragma once
#include "glm/glm.hpp"

namespace ntg
{
  using namespace glm;
  template<length_t vcl = 3, typename vct = float>
  class ray
  {
  public:
    ray() = default;
    ray(const vec<vcl, vct>& origin, const vec<vcl, vct>& direction);

    [[nodiscard]] bool operator==(const ray& other) const;
    [[nodiscard]] bool operator!=(const ray& other) const;
    [[nodiscard]] ray operator*(const mat<vcl, vcl, vct> & transform) const;
    [[nodiscard]] ray operator*(const mat<vcl + 1, vcl + 1, vct> & transform) const;

    vec<vcl, vct> origin;
    vec<vcl, vct> direction;
  };

  using ray2 = ray<2, float>;
  using ray3 = ray<3, float>;
  using ray4 = ray<4, float>;

  using iray2 = ray<2, int>;
  using iray3 = ray<3, int>;
  using iray4 = ray<4, int>;

  using uray2 = ray<2, unsigned>;
  using uray3 = ray<3, unsigned>;
  using uray4 = ray<4, unsigned>;

  using dray2 = ray<2, double>;
  using dray3 = ray<3, double>;
  using dray4 = ray<4, double>;
}