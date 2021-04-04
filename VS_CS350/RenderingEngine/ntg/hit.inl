#pragma once
#include "hit.h"

template <glm::length_t vcl, typename vct>
glm::vec<vcl, vct, glm::defaultp> ntg::hit<vcl, vct>::normal()
{
  return glm::normalize(glm::cross(triangle.points[0] - triangle.points[1], triangle.points[0] - triangle.points[2]));
}

template <glm::length_t vcl, typename vct>
ntg::hit<vcl, vct> ntg::operator*(const mat<vcl, vcl, vct>& transform, const hit<vcl, vct>& h)
{
  return { transform * h.point, transform * h.triangle };
}

template <glm::length_t vcl, typename vct>
ntg::hit<vcl, vct> ntg::operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const hit<vcl, vct>& h)
{
  return {
    transform * glm::vec<vcl + 1, vct>(h.point, 1),
    transform * h.triangle
  };
}
