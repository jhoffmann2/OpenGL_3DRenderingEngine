#pragma once
#include "bounds.h"
#include <algorithm>
#include <iostream>

namespace ntg
{
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
    return valid() ? half_size() + min : vec<vcl, vct>(0);
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds<vcl, vct>::half_size() const
  {
    return valid() ? static_cast<vct>(0.5f) * size() : vec<vcl, vct>(0);
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds<vcl, vct>::size() const
  {
    return valid() ? max - min : vec<vcl, vct>(0);
  }

  template <length_t vcl, typename vct>
  bool bounds<vcl, vct>::valid() const
  {
    for (size_t i = 0; i < vcl; ++i)
      if (max[i] < min[i])
        return false;
    return true;
  }

  template <length_t vcl, typename vct>
  bounds_vertex_iterator<vcl, vct> bounds<vcl, vct>::begin() const
  {
    return { *this };
  }

  template <length_t vcl, typename vct>
  bounds_vertex_iterator<vcl, vct> bounds<vcl, vct>::end()
  {
    return {};
  }

  template <length_t vcl, typename vct>
  bool bounds<vcl, vct>::operator==(const bounds& other) const
  {
    return (min == other.min && max == other.max) || (!valid() && !other.valid());
  }

  template <length_t vcl, typename vct>
  bool bounds<vcl, vct>::operator!=(const bounds& other) const
  {
    return !(*this == other);
  }

  template<length_t vcl, typename vct>
  bounds<vcl, vct> operator*(const mat<vcl, vcl, vct>& transform, const bounds<vcl, vct>& bounds)
  {
    return { bounds.min * transform, bounds.max * transform };
  }

  template<length_t vcl, typename vct>
  bounds<vcl, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const bounds<vcl, vct>& bounds)
  {
    return {
      transform * glm::vec<vcl + 1, vct>(bounds.min, 1),
      transform * glm::vec<vcl + 1, vct>(bounds.max, 1)
    };
  }


  template <length_t vcl, typename vct>
  std::ostream& operator<<(std::ostream& os, const bounds<vcl, vct>& b)
  {
    for (size_t i = 0; i < vcl; ++i)
    {
      os << b.min[i] << " ";
    }
    for (size_t i = 0; i < vcl; ++i)
    {
      os << b.max[i] << " ";
    }
    return os;
  }

  template <length_t vcl, typename vct>
  std::istream& operator>>(std::istream& os, bounds<vcl, vct>& b)
  {
    os >> b.min >> b.max;
    return os;
  }
}
