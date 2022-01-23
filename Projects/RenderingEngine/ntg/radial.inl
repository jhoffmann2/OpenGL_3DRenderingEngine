#pragma once
#include "radial.h"
#include "bounds.h"

#include "glm/gtx/component_wise.hpp"
#include <iosfwd>
#include <glm/gtx/norm.hpp>

namespace ntg
{
  template <length_t vcl, typename vct>
  radial<vcl, vct>::radial(const vec<vcl, vct>& c, vct r) :
    center(c), radius(r)
  {}

  template <length_t vcl, typename vct>
  radial<vcl, vct>::radial(const bounds<vcl, vct>& aabb) :
    center(aabb.center()), radius(glm::length(aabb.half_size()))
  {}

  template <length_t vcl, typename vct>
  radial<vcl, vct> radial<vcl, vct>::grow(const vec<vcl, vct>& p) const
  {
    const vec<vcl, vct>d = p - center;
    const vct l2 = glm::length2(d);
    if (radius * radius < l2)
    {
      radial<vcl, vct> out;
      out.radius = .5f * (radius + sqrt(l2));
      out.center = center + (out.radius - radius) * glm::normalize(d);
      return out;
    }
    return *this;
  }

  template <length_t vcl, typename vct>
  radial<vcl, vct> radial<vcl, vct>::grow(const radial& other) const
  {
    radial<vcl, vct> out;
    const vec<vcl, vct>d = other.center - center;
    out.radius = .5f * (radius + other.radius + glm::length(d));
    out.center = center + (out.radius - radius) * glm::normalize(d);
    return out;
  }

  template <length_t vcl, typename vct>
  radial<vcl, vct> radial<vcl, vct>::grow(const bounds<vcl, vct>& aabb) const
  {
    const vec<vcl, vct>d_max;
    const vct l2_max = std::numeric_limits<vct>::lowest();
    for (const vec<vcl, vct>& vertex : aabb)
    {
      const vec<vcl, vct>d = vertex - center;
      const vct l2 = glm::length2(d);
      if (l2 > l2_max)
      {
        d_max = d;
        l2_max = l2;
      }
    }
    if (radius * radius < l2_max)
    {
      radial<vcl, vct> out;
      out.radius = .5f * (radius + sqrt(l2_max));
      out.center = center + (out.radius - radius) * glm::normalize(d_max);
      return out;
    }
    return *this;
  }

  template <length_t vcl, typename vct>
  vct radial<vcl, vct>::diameter() const
  {
    return valid() ? radius * 2 : 0;
  }

  template <length_t vcl, typename vct>
  vct radial<vcl, vct>::radius2() const
  {
    return valid() ? radius * radius : 0;
  }

  template <length_t vcl, typename vct>
  bool radial<vcl, vct>::valid() const
  {
    return radius >= 0;
  }

  template <length_t vcl, typename vct>
  bool radial<vcl, vct>::operator==(const radial& other)
  {
    return (center == other.center && radius == other.radius) || (!valid() && !other.valid());
  }

  template <length_t vcl, typename vct>
  bool radial<vcl, vct>::operator!=(const radial& other)
  {
    return !(*this == other);
  }

  template <length_t vcl, typename vct>
  template <length_t vml>
  vct radial<vcl, vct>::extractScale(const mat<vml, vml, vct>& t)
  {
    static_assert(vcl == vml || vcl == vml + 1);
    vec<vml - 1, vct> s;
    for (length_t i = 0; i < vml - 1; ++i)
      s[i] = glm::length2(t[i]);
    return glm::sqrt(glm::compMax(s));
  }

  template<length_t vcl, typename vct>
  radial<vcl, vct> operator*(const mat<vcl, vcl, vct>& transform, const radial<vcl, vct>& r)
  {
    return {
      transform * r.center,
      extractScale(transform) * r.radius
    };
  }

  template<length_t vcl, typename vct>
  radial<vcl, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const radial<vcl, vct>& r)
  {
    return {
      transform * glm::vec<vcl + 1, vct>(r.center),
      extractScale(transform) * r.radius
    };
  }

  template <length_t vcl, typename vct>
  std::ostream& operator<<(std::ostream& os, const radial<vcl, vct>& b)
  {
    for (size_t i = 0; i < vcl; ++i)
    {
      os << b.center[i] << " ";
    }
    os << b.radius;
    return os;
  }

  template <length_t vcl, typename vct>
  std::istream& operator>>(std::istream& os, radial<vcl, vct>& b)
  {
    os >> b.center >> b.radius;
    return os;
  }
}