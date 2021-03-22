#pragma once
#include "ray.h"

namespace ntg
{
  template <length_t vcl, typename vct>
  ray<vcl, vct>::ray(const vec<vcl, vct>& origin, const vec<vcl, vct>& direction) :
    origin(origin), direction(direction)
  {}

  template <length_t vcl, typename vct>
  bool ray<vcl, vct>::operator==(const ray& other) const
  {
    return direction == other.direction && origin == other.origin;
  }

  template <length_t vcl, typename vct>
  bool ray<vcl, vct>::operator!=(const ray& other) const
  {
    return direction != other.direction || origin != other.origin;
  }

  template <length_t vcl, typename vct>
  ray<vcl, vct> operator*(const mat<vcl, vcl, vct>& transform, const ray<vcl, vct>& r)
  {
    return {transform * r.direction, transform * r.origin };
  }

  template <length_t vcl, typename vct>
  ray<vcl, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const ray<vcl, vct>& r)
  {
    return {
      transform * glm::vec<vcl + 1, vct>(r.direction, 0),
      transform * glm::vec<vcl + 1, vct>(r.origin, 1)
    };
  }
}
