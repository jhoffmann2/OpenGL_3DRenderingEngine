#pragma once
#include <algorithm>
#include <array>
#include "simplex.h"
#include "type_mat1x1.inl"

namespace ntg
{
  using namespace glm;
  template <length_t vcl, length_t point_count, typename vct>
  simplex<vcl, point_count, vct>::simplex(const std::initializer_list<vec<vcl, vct>>& l)
  {
    assert(l.size() == point_count);
    std::copy(l.begin(), l.end(), points);
  }

  template <length_t vcl, length_t point_count, typename vct>
  simplex<vcl, point_count, vct>::simplex(const mat<point_count, vcl, vct>& pointlist)
  {
    std::copy(std::begin(pointlist), std::end(pointlist), points);
  }

  template <length_t vcl, length_t point_count, typename vct>
  simplex<vcl, point_count, vct>::simplex(const std::array<vec<vcl, vct>, point_count>& pointlist)
  {
    std::copy(std::begin(pointlist), std::end(pointlist), points);
  }

  template <length_t vcl, length_t point_count, typename vct>
  simplex<vcl, point_count, vct>::simplex(const vec<vcl, vct>(&pointArr)[point_count])
  {
    std::copy(std::begin(pointArr), std::end(pointArr), points);
  }

  template <length_t vcl, length_t point_count, typename vct>
  vec<point_count, vct> simplex<vcl, point_count, vct>::toBarycentric(const vec<vcl, vct>& world) const
  {
    // use cramer's rule to solve the system of equations
    std::array<vec<vcl, vct>, point_count> vectors;
    vectors[0] = world - points[0];
    for (size_t i = 1; i < point_count; ++i)
      vectors[i] = points[i] - points[0];

    std::array<vec<point_count - 1, vct>, point_count> row;
    for (size_t i = 0; i < point_count; ++i)
    {
      for (size_t j = 0; j < point_count - 1; ++j)
      {
        row[i][j] = glm::dot(vectors[i], vectors[j + 1]);
      }
    }

    mat<point_count - 1, point_count - 1, vct> dm;
    for (size_t j = 0; j < point_count - 1; ++j)
    {
      dm[j] = row[j + 1];
    }
    vct d = glm::determinant(dm);

    vec<point_count, vct> output;
    output[0] = 1;
    for (size_t i = 0; i < point_count - 1; ++i)
    {
      mat<point_count - 1, point_count - 1, vct> dmi = dm;
      dmi[i] = row[0];
      vct di = glm::determinant(dmi);
      output[i + 1] = di / d;
      output[0] -= output[i + 1];
    }
    return output;
  }

  template <length_t vcl, length_t point_count, typename vct>
  vec<point_count - 1, vct> simplex<vcl, point_count, vct>::toBarycentric_reduced(const vec<vcl, vct>& world) const
  {
    // use cramer's rule to solve the system of equations
    std::array<vec<vcl, vct>, point_count> vectors;
    vectors[0] = world - points[0];
    for (size_t i = 1; i < point_count; ++i)
      vectors[i] = points[i] - points[0];

    std::array<vec<point_count - 1, vct>, point_count> row;
    for (size_t i = 0; i < point_count; ++i)
    {
      for (size_t j = 0; j < point_count - 1; ++j)
      {
        row[i][j] = glm::dot(vectors[i], vectors[j + 1]);
      }
    }

    mat<point_count - 1, point_count - 1, vct> dm;
    for (size_t j = 0; j < point_count - 1; ++j)
    {
      dm[j] = row[j + 1];
    }
    vct d = glm::determinant(dm);

    vec<point_count-1, vct> output;
    output[0] = 1;
    for (size_t i = 0; i < point_count - 1; ++i)
    {
      mat<point_count - 1, point_count - 1, vct> dmi = dm;
      dmi[i] = row[0];
      vct di = glm::determinant(dmi) / d;
      if(i < point_count - 2)
        output[i + 1] = di;
      output[0] -= di;
    }
    return output;
  }

  template <length_t vcl, length_t point_count, typename vct>
  vec<vcl, vct> simplex<vcl, point_count, vct>::toWorld(const vec<point_count, vct>& barycentric) const
  {
    vec<vcl, vct> out(0);
    for (size_t i = 0; i < point_count; ++i)
      out += barycentric[i] * points[i];
    return out;
  }

  template <length_t vcl, length_t point_count, typename vct>
  vec<vcl, vct> simplex<vcl, point_count, vct>::toWorld(const vec<point_count - 1, vct>& barycentric) const
  {
    vec<vcl, vct> out(0);
    vct barycentric_last = 1;
    for (size_t i = 0; i < point_count-1; ++i)
    {
      out += barycentric[i] * points[i];
      barycentric_last -= barycentric[i];
    }
    out += barycentric_last * points[point_count - 1];
    return out;
  }

  template <length_t vcl, length_t point_count, typename vct>
  bool simplex<vcl, point_count, vct>::operator==(const simplex& other) const
  {
    for (size_t i = 0; i < point_count; ++i)
      if (points[i] != other.points[i])
        return false;
    return true;
  }

  template <length_t vcl, length_t point_count, typename vct>
  bool simplex<vcl, point_count, vct>::operator!=(const simplex& other) const
  {
    for (size_t i = 0; i < point_count; ++i)
      if (points[i] != other.points[i])
        return true;
    return false;
  }

  template <length_t vcl, length_t pc, typename vct>
  simplex<vcl, pc, vct> operator*(const mat<vcl, vcl, vct>& transform, simplex<vcl, pc, vct> s)
  {
    for (vec<vcl, vct>& point : s.points)
      point = transform * point;
    return s;
  }

  template <length_t vcl, length_t pc, typename vct>
  simplex<vcl, pc, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, simplex<vcl, pc, vct> s)
  {
    for (vec<vcl, vct>& point : s.points)
      point = transform * vec<vcl + 1, vct>(point, 1);
    return s;
  }
}
