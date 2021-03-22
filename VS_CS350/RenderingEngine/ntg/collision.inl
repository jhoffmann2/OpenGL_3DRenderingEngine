#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "collision.h"
#include "bounds.inl"
#include "radial.inl"
#include "ray.inl"
#include "simplex.inl"
#include "hyperplane.inl"

namespace ntg
{
  using namespace glm;

  template <length_t vcl, typename vct>
  vct distance2(const bounds<vcl, vct>& b, const vec<vcl, vct>& p)
  {
    vct out = 0;
    for (size_t i = 0; i < vcl; ++i)
    {
      vct v = p[i];
      if (v < b.min[i]) out += (b.min[i] - v) * (b.min[i] - v);
      if (v > b.max[i]) out += (v - b.max[i]) * (v - b.max[i]);
    }
    return out;
  }

  template <length_t vcl, typename vct>
  vct distance2(const vec<vcl, vct>& p, const bounds<vcl, vct>& b)
  {
    return distance2(b, p);
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const radial<vcl, vct>& r2)
  {
    const vct combinedR = (r.radius + r2.radius) * (r.radius + r2.radius);
    const vct distance = glm::distance2(r.center, r2.center);
    return distance - combinedR < 1e-6;
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const radial<vcl, vct>& r)
  {
    return distance2(b, r.center) - (r.radius * r.radius) < 1e-6;
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const bounds<vcl, vct>& b)
  {
    return collide(b, r);
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const bounds<vcl, vct>& b2)
  {
    for (size_t i = 0; i < vcl; ++i)
    {
      if (b.max[i] < b2.min[i] || b2.max[i] < b.min[i])
      {
        return false;
      }
    }
    return true;
  }

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const radial<vcl, vct>& r)
  {
    return glm::distance2(r.center, p) - r.radius2() < 1e-6;
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const vec<vcl, vct>& p)
  {
    return collide(p, r);
  }

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const bounds<vcl, vct>& b)
  {
    for (size_t i = 0; i < vcl; ++i)
    {
      if (b.max[i] - p[i] < 1e-6)
        return false;
      if (p[i] - b.min[i] < 1e-6)
        return false;
    }
    return true;
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const vec<vcl, vct>& p)
  {
    return collide(p, b);
  }

  template <length_t vcl, length_t pc, typename vct>
  bool collide(const vec<vcl, vct>& p, const simplex<vcl, pc, vct>& s)
  {
    const vec<pc-1, vct> barycentic = s.toBarycentric_reduced(p);
    vct final_coordinate = 1;
    for (size_t i = 0; i < pc-1; ++i)
    {
      if (1 - barycentic[i] < -1e-6)
        return false;
      if (barycentic[i] < -1e-6)
        return false;
      final_coordinate -= barycentic[i];
    }
    if (1 - final_coordinate < -1e-6)
      return false;
    if (final_coordinate < -1e-6)
      return false;

    // make sure that p wasn't projected
    return glm::distance2(s.toWorld(barycentic), p) < 1e-6;
  }

  template <length_t vcl, length_t pc, typename vct>
  bool collide(const simplex<vcl, pc, vct>& s, const vec<vcl, vct>& p)
  {
    return collide(p, s);
  }

  template <length_t vcl, typename vct>
  bool collide(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h)
  {
    return glm::abs(glm::dot(p - h.origin, h.normal)) < 0.001f;
  }

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p)
  {
    return collide(p, h);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const hyperplane<vcl, vct>& h, vct& t_out)
  {
    const vct denom = glm::dot(r.direction, h.normal);
    if (glm::abs(denom) < 1e-6) // check for parallel
      return false;
    const vec<vcl, vct> diff = h.origin - r.origin;
    t_out = glm::dot(diff, h.normal) / denom;
    return t_out >= 0;
  }

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const ray<vcl, vct>& r, vct& t_out)
  {
    return collide(h, r, t_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const bounds<vcl, vct>& b, vct& tmin_out, vct& tmax_out)
  {
    tmin_out = -std::numeric_limits<vct>::infinity();
    tmax_out = std::numeric_limits<vct>::infinity();
    for (size_t i = 0; i < vcl; ++i)
    {
      if (r.direction[i] != 0)
      {
        vct t1 = (b.min[0] - r.origin[0]) / r.direction[0];
        vct t2 = (b.max[0] - r.origin[0]) / r.direction[0];

        tmin_out = glm::max(tmin_out, glm::min(t1, t2));
        tmax_out = glm::min(tmax_out, glm::max(t1, t2));
      }
    }
    return tmax_out >= tmin_out;
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const ray<vcl, vct>& r, vct& tmin_out, vct& tmax_out)
  {
    return collide(r, b, tmin_out, tmax_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const bounds<vcl, vct>& b, simplex<vcl, 2, vct>& s_out)
  {
    vct tmin, tmax;
    if(collide(r, b, tmin, tmax))
    {
      s_out = simplex<vcl, 2, vct>(
        r.origin + tmin * r.direction, 
        r.origin + tmax * r.direction
      );
      return true;
    }
    return false;
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const ray<vcl, vct>& r, simplex<vcl, 2, vct>& s_out)
  {
    return collide(r,b,s_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& ry, const radial<vcl, vct>& rd, vct& tmin_out, vct& tmax_out)
  {
    const vec<vcl, vct> oc = ry.origin - rd.center;
    const vct a = glm::length2(ry.direction);
    const vct b = 2 * glm::dot(oc, ry.direction);
    const vct c = glm::length2(oc) - rd.radius * rd.radius;
    const vct discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
      return false;
    const vct sq = glm::sqrt(discriminant);
    tmin_out = (-b - sq) / (2 * a);
    tmax_out = (-b + sq) / (2 * a);
    return tmin_out > 0.f && tmax_out > 0.f;
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& rd, const ray<vcl, vct>& ry, vct& tmin_out, vct& tmax_out)
  {
    return collide(ry, rd, tmin_out, tmax_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& ry, const radial<vcl, vct>& rd, simplex<vcl, 2, vct>& s_out)
  {
    vct tmin, tmax;
    if (collide(ry, rd, tmin, tmax))
    {
      s_out = simplex<vcl, 2, vct>(
        ry.origin + tmin * ry.direction,
        ry.origin + tmax * ry.direction
        );
      return true;
    }
    return false;
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& rd, const ray<vcl, vct>& ry, simplex<vcl, 2, vct>& s_out)
  {
    return collide(ry,rd,s_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl, vct>& s, vct& t_out)
  {
    hyperplane<vcl, vct> h(s);
    if (collide(h, r, t_out))
    {
      vec<vcl, vct> p = r.origin + t_out * r.direction;
      for (vct barycentric : s.toBarycentric(p))
        if (barycentric > 1 || barycentric < 0)
          return false;
      return true;
    }
    return false;
  }

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl, vct>& s, const ray<vcl, vct>& r, vct& t_out)
  {
    return collide(r, s, t_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl + 1, vct>& s, vct& tmin_out, vct& tmax_out)
  {
    tmin_out = std::numeric_limits<vct>::max();
    tmax_out = std::numeric_limits<vct>::lowest();
    bool collsion = false;
    // construct n lower dimension simplexes out of each face and raycast onto those instead
    for (size_t i = 0; i < vcl; ++i)
    {
      simplex<vcl, vcl, vct> s2;
      int k = 0;
      for (size_t j = 0; j < vcl; ++j)
      {
        if (i == j) continue;
        s2.points[k++] = s.points[j];
      }
      vct t;
      if (collide(s2, r, t))
      {
        tmin_out = glm::min(tmin_out, t);
        tmax_out = glm::min(tmax_out, t);
        collsion = true;
      }
    }
    return collsion;
  }

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl + 1, vct>& s, const ray<vcl, vct>& r, vct& tmin_out, vct& tmax_out)
  {
    return collision(r, s, tmin_out, tmax_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const ray<vcl, vct>& r, const simplex<vcl, vcl + 1, vct>& s, simplex<vcl, 2, vct>& s_out)
  {
    vct tmin, tmax;
    if (collide(r, s, tmin, tmax))
    {
      s_out = simplex<vcl, 2, vct>(
        r.origin + tmin * r.direction,
        r.origin + tmax * r.direction
        );
      return true;
    }
    return false;
  }

  template <length_t vcl, typename vct>
  bool collide(const simplex<vcl, vcl + 1, vct>& s, const ray<vcl, vct>& r, simplex<vcl, 2, vct>& s_out)
  {
    return collide(r,s,s_out);
  }

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const bounds<vcl, vct>& b)
  {
    bool infront = false;
    bool behind = false;
    for (const vec<vcl, vct>& vertex : b)
    {
      const vct dotp = glm::dot(h.normal, vertex - h.origin);
      if (dotp >= 0) infront = true;
      if (dotp <= 0) behind = true;
      if (infront && behind) return true;
    }
    return false;
  }

  template <length_t vcl, typename vct>
  bool collide(const bounds<vcl, vct>& b, const hyperplane<vcl, vct>& h)
  {
    return collide(h, b);
  }

  template <length_t vcl, typename vct>
  bool collide(const hyperplane<vcl, vct>& h, const radial<vcl, vct>& r)
  {
    return dot(glm::normalize(h.normal), r.center - h.origin) <= r;
  }

  template <length_t vcl, typename vct>
  bool collide(const radial<vcl, vct>& r, const hyperplane<vcl, vct>& h)
  {
    return collide(h, r);
  }

  template <length_t vcl, typename vct>
  bool in_front(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h)
  {
    return glm::dot(p - h.origin, h.normal) > 0;
  }

  template <length_t vcl, typename vct>
  bool in_front(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p)
  {
    return in_front(p,h);
  }


  template <length_t vcl, typename vct>
  int ternary_collide(const vec<vcl, vct>& p, const hyperplane<vcl, vct>& h)
  {
    const float d = glm::dot(p - h.origin, h.normal);
    return (glm::abs(d) < 1e-6)? 0 : static_cast<int>(sign(d));
  }

  template <length_t vcl, typename vct>
  int ternary_collide(const hyperplane<vcl, vct>& h, const vec<vcl, vct>& p)
  {
    return ternary_collide(p,h);
  }

  inline bool collide(const lineseg3& l, const hyperplane3& p, vec3& hit_out)
  {
    float t;
    const ray3 r{ l.points[0], l.points[1] - l.points[0] };
    if(collide(r, p, t))
    {
      if(t > 1) return false;
      if(t < 0) return false;
      hit_out = r.origin + r.direction * t;
      return true;
    }
    return false;
  }

  inline bool split(const triangle3& t, const hyperplane3& p, std::vector<triangle3>& front_out,
                    std::vector<triangle3>& back_out)
  {
    const lineseg3 edges[3] = {
      {t.points[0], t.points[1]},
      {t.points[1], t.points[2]},
      {t.points[2], t.points[0]},
    };

    vec3 frontlist[4];
    vec3 backlist[4];
    size_t frontSize = 0;
    size_t backSize = 0;

    for (const lineseg3 &edge : edges)
    {
      const int tc0 = ternary_collide(edge.points[0], p) + 1;
      const int tc1 = ternary_collide(edge.points[1], p) + 1;
      const int state = (tc0 | tc1 << 2);
      switch(state)
      {
      case 1:  // C, B
        backlist[backSize] = edge.points[0];
        ++backSize;
      case 0:  // B, B
        backlist[backSize] = edge.points[1];
        ++backSize;
        break;
      case 4:  // B, C
        backlist[backSize] = edge.points[1];
        ++backSize;
      case 6:  // F, C
      case 5:  // C, C
      case 10: // F, F
      case 9:  // C, F
        frontlist[frontSize] = edge.points[1];
        ++frontSize;
        break;
      case 2:  // F, B
        {
          vec3 hit;
          collide(edge, p, hit);
          frontlist[frontSize] = hit;
          ++frontSize;
          backlist[backSize] = hit;
          ++backSize;
          backlist[backSize] = edge.points[1];
          ++backSize;
        }
        break;
      case 8:  // B, F
        {
          vec3 hit;
          collide(edge, p, hit);
          frontlist[frontSize] = hit;
          ++frontSize;
          frontlist[frontSize] = edge.points[1];
          ++frontSize;
          backlist[backSize] = hit;
          ++backSize;
        }
        break;
      default:
        throw std::out_of_range("invalid jump table value");
      }
    }
    for(size_t i = 0; i < frontSize; ++i)
      assert(ternary_collide(frontlist[i], p) >= 0);
    for(size_t i = 0; i < backSize; ++i)
      assert(ternary_collide(backlist[i], p) <= 0);

    if(frontSize >= 3)
      front_out.emplace_back(triangle3{frontlist[0], frontlist[1], frontlist[2]});
    if (frontSize == 4)
      front_out.emplace_back(triangle3{frontlist[0], frontlist[2], frontlist[3]});
    if(backSize >= 3)
      back_out.emplace_back(triangle3{backlist[0], backlist[1], backlist[2]});
    if (backSize == 4)
      back_out.emplace_back(triangle3{backlist[0], backlist[2], backlist[3]});
    return frontSize >= 3 && backSize >= 3;
  }
}
