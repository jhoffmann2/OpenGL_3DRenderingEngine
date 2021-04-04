#pragma once
#include <glm/glm.hpp>

namespace ntg
{
  using namespace glm;
  template<length_t vcl = 3, length_t point_count = vcl, typename vct = float>
  class simplex
  {
  public:
    static_assert(vcl >= point_count-1); // check for impossible geometry

    simplex() = default;
    simplex(const std::initializer_list<vec<vcl, vct>>& l);
    explicit simplex(const mat<point_count,vcl,vct>& pointlist);
    explicit simplex(const std::array<vec<vcl,vct>,point_count>& pointlist);
    explicit simplex(const vec<vcl, vct> (&pointArr)[point_count]);

    // converts input coordinate into local barycentric coordinates
    // note: if the input point doesn't exist within this space, it will be projected
    [[nodiscard]] vec<point_count, vct> toBarycentric(const vec<vcl, vct> &world) const;
    [[nodiscard]] vec<point_count-1, vct> toBarycentric_reduced(const vec<vcl, vct> &world) const;
    [[nodiscard]] vec<vcl, vct> toWorld(const vec<point_count, vct> &barycentric) const;
    // automatically determine the last barycentric coordinate based on sum to 1 property
    [[nodiscard]] vec<vcl, vct> toWorld(const vec<point_count-1, vct> &barycentric) const;

    [[nodiscard]] bool operator==(const simplex& other) const;
    [[nodiscard]] bool operator!=(const simplex& other) const;

    vec<vcl, vct> points[point_count]{vec<vcl, vct>{0}};
  };

  template<length_t vcl = 3, length_t pc = vcl, typename vct = float>
  [[nodiscard]] simplex<vcl, pc, vct> operator*(const mat<vcl, vcl, vct>& transform, simplex<vcl, pc, vct> s);

  template<length_t vcl = 3, length_t pc = vcl, typename vct = float>
  [[nodiscard]] simplex<vcl, pc, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, simplex<vcl, pc, vct> s);

  template<typename vct = float>
  vct area(const simplex<3,3,vct> &tri);

  using lineseg1 = simplex<1, 2, float>;
  using lineseg2 = simplex<2, 2, float>;
  using lineseg3 = simplex<3, 2, float>;
  using lineseg4 = simplex<4, 2, float>;

  using ilineseg1 = simplex<1, 2, int>;
  using ilineseg2 = simplex<2, 2, int>;
  using ilineseg3 = simplex<3, 2, int>;
  using ilineseg4 = simplex<4, 2, int>;

  using ulineseg1 = simplex<1, 2, unsigned>;
  using ulineseg2 = simplex<2, 2, unsigned>;
  using ulineseg3 = simplex<3, 2, unsigned>;
  using ulineseg4 = simplex<4, 2, unsigned>;

  using dlineseg1 = simplex<1, 2, double>;
  using dlineseg2 = simplex<2, 2, double>;
  using dlineseg3 = simplex<3, 2, double>;
  using dlineseg4 = simplex<4, 2, double>;

  using triangle2 = simplex<2, 3, float>;
  using triangle3 = simplex<3, 3, float>;
  using triangle4 = simplex<4, 3, float>;

  using itriangle2 = simplex<2, 3, int>;
  using itriangle3 = simplex<3, 3, int>;
  using itriangle4 = simplex<4, 3, int>;

  using utriangle2 = simplex<2, 3, unsigned>;
  using utriangle3 = simplex<3, 3, unsigned>;
  using utriangle4 = simplex<4, 3, unsigned>;

  using dtriangle2 = simplex<2, 3, double>;
  using dtriangle3 = simplex<3, 3, double>;
  using dtriangle4 = simplex<4, 3, double>;

  using pyramid3 = simplex<3, 4, float>;
  using pyramid4 = simplex<4, 4, float>;

  using ipyramid3 = simplex<3, 4, int>;
  using ipyramid4 = simplex<4, 4, int>;
  
  using upyramid3 = simplex<3, 4, unsigned>;
  using upyramid4 = simplex<4, 4, unsigned>;

  using dpyramid3 = simplex<3, 4, double>;
  using dpyramid4 = simplex<4, 4, double>;
}