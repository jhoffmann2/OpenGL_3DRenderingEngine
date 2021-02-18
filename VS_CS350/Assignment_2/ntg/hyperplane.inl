#pragma once
#include "hyperplane.h"
#include "simplex.inl"

namespace ntg
{
  template <length_t vcl, typename vct>
  hyperplane<vcl, vct>::hyperplane(const vec<vcl, vct>& origin, const vec<vcl, vct>& normal) :
    origin(origin), normal(normal)
  {}

  // returns vcl+1 linearly dependent vectors
  template <length_t vcl, typename vct>
  constexpr std::array<vec<vcl, vct>, vcl> linearly_independent()
  {
    std::array<vec<vcl, vct>, vcl> trials{vec<vcl, vct>(0)};
    for(size_t i = 0; i < vcl; ++i)
    {
      trials[i][i] = static_cast<vct>(8475947); // arbitrarily rare number
    }
    return trials;
  }

  template <length_t vcl, typename vct>
  hyperplane<vcl, vct>::hyperplane(const mat<vcl, vcl, vct>& points) : origin(0)
  {
    if constexpr(vcl == 2)
    {
      *this = hyperplaneFromPoints(points[0], points[1]);
      return;
    }
    if constexpr(vcl == 3)
    {
      *this = hyperplaneFromPoints(points[0], points[1], points[2]);
      return;
    }

    // at least one of these points is garenteed to not exist on the hyperplane
    auto trials = linearly_independent<vcl,vct>();
    simplex s(points);
    for (const vec<vcl,vct> &trial : trials)
    {
      const vec<vcl, vct> projected = s.toWorld(s.toBarycentric(trial));
      vec<vcl, vct> n = trial - projected;
      if(abs(glm::dot(n, vec<vcl, vct>{1})) > 1e-6)
      {
        normal = glm::normalize(n);
        origin = points[0];
        return;
      }
    }
    normal = glm::vec<vcl,vct>{glm::sqrt(static_cast<vct>(vcl))};
  }

  template <length_t vcl, typename vct>
  hyperplane<vcl, vct>::hyperplane(const std::array<vec<vcl, vct>, vcl>& points)
  {
    if constexpr (vcl == 2)
    {
      *this = hyperplaneFromPoints(points[0], points[1]);
      return;
    }
    if constexpr (vcl == 3)
    {
      *this = hyperplaneFromPoints(points[0], points[1], points[2]);
      return;
    }

    // at least one of these points is garenteed to not exist on the hyperplane
    auto trials = linearly_independent<vcl, vct>();
    simplex s(points);
    for (const vec<vcl, vct>& trial : trials)
    {
      const vec<vcl, vct> projected = s.toWorld(s.toBarycentric(trial));
      vec<vcl, vct> n = trial - projected;
      if (abs(glm::dot(n, vec<vcl, vct>{1})) > 1e-6)
      {
        normal = glm::normalize(n);
        origin = points[0];
        return;
      }
    }
    normal = glm::vec<vcl, vct>{ sqrt(static_cast<vct>(vcl)) };
  }

  template <length_t vcl, typename vct>
  hyperplane<vcl, vct>::hyperplane(const simplex<vcl, vcl, vct>& s)
  {
    if constexpr (vcl == 2)
    {
      *this = hyperplaneFromPoints(s.points[0], s.points[1]);
      return;
    }
    if constexpr (vcl == 3)
    {
      *this = hyperplaneFromPoints(s.points[0], s.points[1], s.points[2]);
      return;
    }

    // at least one of these points is garenteed to not exist on the hyperplane
    auto trials = linearly_independent<vcl, vct>();
    for (const vec<vcl, vct>& trial : trials)
    {
      const vec<vcl, vct> projected = s.toWorld(s.toBarycentric(trial));
      vec<vcl, vct> n = trial - projected;
      if (abs(glm::dot(n, vec<vcl, vct>{1})) > 1e-6)
      {
        normal = glm::normalize(n);
        origin = s.points[0];
        return;
      }
    }
    normal = glm::vec<vcl, vct>{ sqrt(static_cast<vct>(vcl)) };
  }

  template <length_t vcl, typename vct>
  bool hyperplane<vcl, vct>::operator==(const hyperplane& other) const
  {
    return normal == other.normal && origin == other.origin;
  }

  template <length_t vcl, typename vct>
  bool hyperplane<vcl, vct>::operator!=(const hyperplane& other) const
  {
    return normal != other.normal || origin != other.origin;
  }

  template <length_t vcl, typename vct>
  hyperplane<vcl, vct> hyperplane<vcl, vct>::operator*(const mat<vcl, vcl, vct>& transform) const
  {
    return { normal * transform, origin * transform };
  }

  template <length_t vcl, typename vct>
  hyperplane<vcl, vct> hyperplane<vcl, vct>::operator*(const mat<vcl + 1, vcl + 1, vct>& transform) const
  {
    return {
      glm::vec<vcl + 1, vct>(normal, 0) * transform,
      glm::vec<vcl + 1, vct>(origin, 1) * transform
    };
  }

  template <typename vct>
  hyperplane<2, vct> hyperplaneFromPoints(const vec<2, vct>& p1, const vec<2, vct>& p2)
  {
    return {
      p1,
      glm::normalize(vec<2,vct>{p1.y - p2.y,p2.x - p1.x})
    };
  }

  template <typename vct>
  hyperplane<3, vct> hyperplaneFromPoints(const vec<3, vct>& p1, const vec<3, vct>& p2, const vec<3, vct>& p3)
  {
    return {
      p1,
      glm::normalize(glm::cross(p2-p1, p3-p1))
    };
  }
}
