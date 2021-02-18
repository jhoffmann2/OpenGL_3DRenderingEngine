#pragma once
#include "glm/glm.hpp"
#include "simplex.h"


namespace ntg
{
  using namespace glm;

  template<length_t vcl = 3, typename vct = float>
  class hyperplane
  {
  public:
    hyperplane() = default;
    hyperplane(const vec<vcl, vct> & origin, const vec<vcl, vct> & normal);
    hyperplane(const mat<vcl,vcl,vct> &points); // matrix of points on plane
    hyperplane(const std::array<vec<vcl, vct>,vcl> &points); // points on plane
    hyperplane(const simplex<vcl,vcl,vct>& s); // simplex on plane

    [[nodiscard]] bool operator==(const hyperplane& other) const;
    [[nodiscard]] bool operator!=(const hyperplane& other) const;
    [[nodiscard]] hyperplane operator*(const mat<vcl, vcl, vct> & transform) const;
    [[nodiscard]] hyperplane operator*(const mat<vcl + 1, vcl + 1, vct> & transform) const;

    vec<vcl, vct> origin{0};
    vec<vcl, vct> normal;
  };

  using hyperplane2 = hyperplane<2, float>;
  using hyperplane3 = hyperplane<3, float>;
  using hyperplane4 = hyperplane<4, float>;

  using ihyperplane2 = hyperplane<2, int>;
  using ihyperplane3 = hyperplane<3, int>;
  using ihyperplane4 = hyperplane<4, int>;

  using uhyperplane2 = hyperplane<2, unsigned>;
  using uhyperplane3 = hyperplane<3, unsigned>;
  using uhyperplane4 = hyperplane<4, unsigned>;

  using dhyperplane2 = hyperplane<2, double>;
  using dhyperplane3 = hyperplane<3, double>;
  using dhyperplane4 = hyperplane<4, double>;

  template<typename vct = float>
  hyperplane<2, vct> hyperplaneFromPoints(
    const vec<2, vct>& p1, 
    const vec<2, vct>& p2
  );

  template<typename vct = float>
  hyperplane<3, vct> hyperplaneFromPoints(
    const vec<3, vct>& p1,
    const vec<3, vct>& p2,
    const vec<3, vct>& p3
  );

}
