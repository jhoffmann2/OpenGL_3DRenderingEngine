#pragma once
#include <algorithm>

#include "glm/vec4.hpp"

namespace glm
{
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

    vec<vcl, vct> min = vec<vcl, vct>(std::numeric_limits<vct>::max());
    vec<vcl, vct> max = vec<vcl, vct>(std::numeric_limits<vct>::min());
  };

  template <length_t vcl, typename vct>
  bounds<vcl, vct>::bounds(const vec<vcl, vct>& min, const vec<vcl, vct>& max) :
    min(min), max(max)
  {}

  template <length_t vcl, typename vct>
  bounds<vcl, vct> bounds<vcl, vct>::grow(const vec<vcl, vct>& p) const
  {
    return { glm::min(min, p), glm::max(max, p) };
  }

  template <length_t vcl, typename vct>
  bounds<vcl, vct> bounds<vcl, vct>::grow(const bounds& b) const
  {
    return { glm::min(min, b.min), glm::max(max, b.max) };
  }

  template <length_t vcl, typename vct>
  bounds<vcl, vct> bounds<vcl, vct>::grow_static(const bounds& b, const vec<vcl, vct>& p)
  {
    return b.grow(p);
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds<vcl, vct>::center() const
  {
    return half_size() + min;
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds<vcl, vct>::half_size() const
  {
    return 0.5f * size();
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds<vcl, vct>::size() const
  {
    return max - min;
  }

  template <length_t vcl, typename vct>
  std::ostream &operator<<(std::ostream &os, const bounds<vcl, vct>& b)
  {
    for(size_t i = 0; i < vcl; ++i)
    {
      os << b.min[i] << " ";
    }
    for(size_t i = 0; i < vcl; ++i)
    {
      os << b.max[i] << " ";
    }
    return os;
  }

  template <length_t vcl, typename vct>
  std::istream&operator>>(std::istream &os, bounds<vcl, vct>& b)
  {
    os >> b.min >> b.max;
    return os;
  }


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


