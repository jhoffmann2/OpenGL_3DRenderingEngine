#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <vector>

#include "bounds.h"
#include "radial.h"
#include "ray.h"
#include "simplex.h"
#include "hyperplane.h"

namespace ntg
{
  using namespace glm;

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const radial<vcl, vct>& r2);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const radial<vcl, vct>& r);

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const bounds<vcl, vct>& b);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const bounds<vcl, vct>& b2);

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const radial<vcl, vct>& r);

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const vec<vcl, vct>& p);

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const bounds<vcl, vct>& b);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const vec<vcl, vct>& p);

  template <length_t vcl, length_t pc, typename vct>
  bool collide(const vec<vcl, vct>& p, const simplex<vcl, pc, vct>& s);

  template <length_t vcl, length_t pc, typename vct>
  bool collide(const simplex<vcl, pc, vct>& s, const vec<vcl, vct>& p);

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h);

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const hyperplane<vcl, vct>& h, vct &t_out);

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const ray<vcl, vct>& r, vct& t_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const bounds<vcl, vct>& b, vct &tmin_out, vct &tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const ray<vcl, vct>& r, vct& tmin_out, vct& tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const bounds<vcl, vct>& b, simplex<vcl,2,vct> &s_out);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const ray<vcl, vct>& r, simplex<vcl,2,vct> &s_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& ry, const radial<vcl, vct>& rd, vct& tmin_out, vct& tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& rd, const ray<vcl, vct>& ry, vct& tmin_out, vct& tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& ry, const radial<vcl, vct>& rd, simplex<vcl, 2, vct>& s_out);

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& rd, const ray<vcl, vct>& ry, simplex<vcl, 2, vct>& s_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl, vct>& s, vct& t_out);

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl, vct>& s, const ray<vcl, vct>& r, vct& t_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl+1, vct>& s, vct& tmin_out, vct& tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl+1, vct>& s, const ray<vcl, vct>& r, vct& tmin_out, vct& tmax_out);

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl+1, vct>& s, simplex<vcl, 2, vct>& s_out);

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl+1, vct>& s, const ray<vcl, vct>& r, simplex<vcl, 2, vct>& s_out);

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const bounds<vcl, vct>& b);

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const hyperplane<vcl, vct>& h);

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const radial<vcl, vct>& r);

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const hyperplane<vcl, vct>& h);

  template <length_t vcl, typename vct>
  bool in_front(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h);

  template <length_t vcl, typename vct>
  bool in_front(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p);

  template <length_t vcl, typename vct>
  int ternary_collide(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h);

  template <length_t vcl, typename vct>
  int ternary_collide(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p);

  bool collide(const lineseg3 &l, const hyperplane3& p, vec3 &hit_out);

  bool split(const triangle3 &t, const hyperplane3 &p, std::vector<triangle3> &front_out, std::vector<triangle3> &back_out);
}