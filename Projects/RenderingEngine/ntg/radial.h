#pragma once
#include <glm/glm.hpp>

namespace ntg
{
  using namespace glm;

  template<length_t vcl, typename vct>
  class bounds;

  template<length_t vcl = 3,typename vct = float>
  class radial
  {
  public:
    radial() = default;
    radial(const vec<vcl, vct> &c, vct r);
    explicit radial(const bounds<vcl, vct>& aabb);
    [[nodiscard]] radial grow(const vec<vcl, vct>& p) const;
    [[nodiscard]] radial grow(const radial& other) const;
    [[nodiscard]] radial grow(const bounds<vcl,vct>& aabb) const;

    [[nodiscard]] vct diameter() const;
    [[nodiscard]] vct radius2() const;

    [[nodiscard]] bool valid() const;

    [[nodiscard]] bool operator==(const radial& other);
    [[nodiscard]] bool operator!=(const radial& other);

    vec<vcl, vct> center = vec<vcl, vct>(0);
    vct radius = std::numeric_limits<vct>::lowest();
  private:
    template<length_t vml>
    static vct extractScale(const mat<vml, vml,vct>& t);
  };

  template<length_t vcl = 3, typename vct = float>
  [[nodiscard]] radial<vcl, vct> operator*(const mat<vcl, vcl, vct>& transform, const radial<vcl, vct>& r);

  template<length_t vcl = 3, typename vct = float>
  [[nodiscard]] radial<vcl, vct> operator*(const mat<vcl + 1, vcl + 1, vct>& transform, const radial<vcl, vct>& r);

  using radial2 = radial<2, float>;
  using radial3 = radial<3, float>;
  using radial4 = radial<4, float>;

  using iradial2 = radial<2, int>;
  using iradial3 = radial<3, int>;
  using iradial4 = radial<4, int>;

  using uradial2 = radial<2, unsigned>;
  using uradial3 = radial<3, unsigned>;
  using uradial4 = radial<4, unsigned>;

  using dradial2 = radial<2, double>;
  using dradial3 = radial<3, double>;
  using dradial4 = radial<4, double>;

}
