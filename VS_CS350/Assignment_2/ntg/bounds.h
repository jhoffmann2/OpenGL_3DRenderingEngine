#pragma once
#include "glm/glm.hpp"

namespace ntg
{
  using namespace glm;
  template<length_t vcl, typename vct>
  class bounds_vertex_iterator;

  template<length_t vcl = 3, typename vct = float>
  class bounds
  {
  public:
    bounds() = default;
    bounds(const vec<vcl, vct>& min, const vec<vcl, vct>& max);
    [[nodiscard]] bounds grow(const vec<vcl, vct>& p) const;
    [[nodiscard]] bounds grow(const bounds& b) const;
    [[nodiscard]] static bounds grow_static(const bounds& b, const vec<vcl, vct>& p);

    [[nodiscard]] vec<vcl, vct> center() const;
    [[nodiscard]] vec<vcl, vct> half_size() const;
    [[nodiscard]] vec<vcl, vct> size() const;
    [[nodiscard]] bool valid() const;

    [[nodiscard]] bounds_vertex_iterator<vcl,vct> begin() const;
    [[nodiscard]] static bounds_vertex_iterator<vcl,vct> end();

    [[nodiscard]] bool operator==(const bounds& other) const;
    [[nodiscard]] bool operator!=(const bounds& other) const;
    [[nodiscard]] bounds operator*(const mat<vcl,vcl,vct>& transform) const;
    [[nodiscard]] bounds operator*(const mat<vcl+1,vcl+1,vct>& transform) const;

    vec<vcl, vct> min = vec<vcl, vct>(std::numeric_limits<vct>::max());
    vec<vcl, vct> max = vec<vcl, vct>(std::numeric_limits<vct>::min());
  };

  using bounds2 = bounds<2, float>;
  using bounds3 = bounds<3, float>;
  using bounds4 = bounds<4, float>;

  using ibounds2 = bounds<2, int>;
  using ibounds3 = bounds<3, int>;
  using ibounds4 = bounds<4, int>;

  using ubounds2 = bounds<2, unsigned>;
  using ubounds3 = bounds<3, unsigned>;
  using ubounds4 = bounds<4, unsigned>;

  using dbounds2 = bounds<2, double>;
  using dbounds3 = bounds<3, double>;
  using dbounds4 = bounds<4, double>;
}


