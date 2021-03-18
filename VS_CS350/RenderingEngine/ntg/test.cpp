#include "pch.h"
#include "collision.inl"
#include "glm/gtx/rotate_vector.hpp"

static constexpr float lambda = 1.f / (1 << 8);
static const float isq2 = 1.f / glm::sqrt(2.f);

template <glm::length_t vcl, typename vct>
void radial_radial_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    ntg::radial<vcl,vct> r1 { glm::vec<vcl,vct>{0},6.125 };
    ntg::radial<vcl, vct>r2 { glm::vec<vcl,vct>{0},3.875 };
    while (r1.center[axis] <= 5.f)
    {
      EXPECT_TRUE(ntg::collide(r1, r2));
      r1.center[axis] += lambda;
      r2.center[axis] -= lambda;
    }
    EXPECT_FALSE(ntg::collide(r1, r2));
  }
}

TEST(RadialRadial_SingleAxis_Move, Collision_1D) {

  radial_radial_single_axis_move_test<1,float>();
  radial_radial_single_axis_move_test<1,double>();
}

TEST(RadialRadial_SingleAxis_Move, Collision_2D) {

  radial_radial_single_axis_move_test<2,float>();
  radial_radial_single_axis_move_test<2,double>();
}

TEST(RadialRadial_SingleAxis_Move, Collision_3D) {

  radial_radial_single_axis_move_test<3,float>();
  radial_radial_single_axis_move_test<3,double>();
}

TEST(RadialRadial_SingleAxis_Move, Collision_4D) {

  radial_radial_single_axis_move_test<4,float>();
  radial_radial_single_axis_move_test<4,double>();
}

template <glm::length_t vcl, typename vct>
void radial_radial_two_axis_move_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      ntg::radial<vcl, vct> r1{ glm::vec<vcl,vct>{0},6.125 };
      ntg::radial<vcl, vct>r2{ glm::vec<vcl,vct>{0},3.875 };
      while (r1.center[axis1] <= isq2 * 5.f)
      {
        EXPECT_TRUE(ntg::collide(r1, r2));
        r1.center[axis1] += isq2 * lambda;
        r1.center[axis2] += isq2 * lambda;
        r2.center[axis1] -= isq2 * lambda;
        r2.center[axis2] -= isq2 * lambda;
      }
      EXPECT_FALSE(ntg::collide(r1, r2));
    }
  }
}

TEST(RadialRadial_TwoAxis_Move, Collision_2D) {

  radial_radial_two_axis_move_test<2,float>();
  radial_radial_two_axis_move_test<2,double>();
}

TEST(RadialRadial_TwoAxis_Move, Collision_3D) {

  radial_radial_two_axis_move_test<3,float>();
  radial_radial_two_axis_move_test<3,double>();
}

TEST(RadialRadial_TwoAxis_Move, Collision_4D) {

  radial_radial_two_axis_move_test<4,float>();
  radial_radial_two_axis_move_test<4,double>();
}

template <glm::length_t vcl, typename vct>
void radial_radial_single_axis_expand_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    glm::vec<vcl, vct> c0{ 0 };
    glm::vec<vcl, vct> c1{ 0 };
    c0[axis] = 6.125f;
    c1[axis] = -3.875f;
    ntg::radial<vcl, vct> r1{ c0,0 };
    ntg::radial<vcl, vct>r2{ c1,0 };
    while (r1.radius < 5.f)
    {
      EXPECT_FALSE(ntg::collide(r1, r2));
      r1.radius += lambda;
      r2.radius += lambda;
    }
    EXPECT_TRUE(ntg::collide(r1, r2));
  }
}

TEST(RadialRadial_SingleAxis_Expand, Collision_1D) {

  radial_radial_single_axis_expand_test<1,float>();
  radial_radial_single_axis_expand_test<1,double>();
}

TEST(RadialRadial_SingleAxis_Expand, Collision_2D) {

  radial_radial_single_axis_expand_test<2,float>();
  radial_radial_single_axis_expand_test<2,double>();
}

TEST(RadialRadial_SingleAxis_Expand, Collision_3D) {

  radial_radial_single_axis_expand_test<3,float>();
  radial_radial_single_axis_expand_test<3,double>();
}

TEST(RadialRadial_SingleAxis_Expand, Collision_4D) {

  radial_radial_single_axis_expand_test<4,float>();
  radial_radial_single_axis_expand_test<4,double>();
}

template <glm::length_t vcl, typename vct>
void radial_radial_two_axis_expand_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      glm::vec<vcl, vct> c0{ 0 };
      glm::vec<vcl, vct> c1{ 0 };
      c0[axis1] = isq2 * 6.125f;
      c0[axis2] = isq2 * 6.125f;
      c1[axis1] = isq2 * -3.875f;
      c1[axis2] = isq2 * -3.875f;
      ntg::radial<vcl, vct> r1{ c0,0 };
      ntg::radial<vcl, vct>r2{ c1,0 };
      while (r1.radius < 5.0)
      {
        EXPECT_FALSE(ntg::collide(r1, r2));
        r1.radius += lambda;
        r2.radius += lambda;
      }
      EXPECT_TRUE(ntg::collide(r1, r2));
    }
  }
}

TEST(RadialRadial_TwoAxis_Expand, Collision_2D) {

  radial_radial_two_axis_expand_test<2,float>();
  radial_radial_two_axis_expand_test<2,double>();
}

TEST(RadialRadial_TwoAxis_Expand, Collision_3D) {

  radial_radial_two_axis_expand_test<3,float>();
  radial_radial_two_axis_expand_test<3,double>();
}

TEST(RadialRadial_TwoAxis_Expand, Collision_4D) {

  radial_radial_two_axis_expand_test<4,float>();
  radial_radial_two_axis_expand_test<4,double>();
}

template <glm::length_t vcl, typename vct>
void bounds_bounds_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    glm::vec<vcl, vct> v1{5.f};
    v1[axis] = 6.125f;
    glm::vec<vcl, vct> v2{5.f};
    v2[axis] = 3.875f;
    ntg::bounds<vcl, vct> b1{ -v1, v1 };
    ntg::bounds<vcl, vct> b2{ -v2, v2 };
    while (b1.center()[axis] - b2.center()[axis] <= 10.f)
    {
      EXPECT_TRUE(ntg::collide(b1, b2));
      b1.min[axis] += lambda;
      b1.max[axis] += lambda;
      b2.min[axis] -= lambda;
      b2.max[axis] -= lambda;
    }
    EXPECT_FALSE(ntg::collide(b1, b2));
  }
}

TEST(BoundsBounds_SingleAxis_Move, Collision_1D) {

  bounds_bounds_single_axis_move_test<1, float>();
  bounds_bounds_single_axis_move_test<1, double>();
}

TEST(BoundsBounds_SingleAxis_Move, Collision_2D) {

  bounds_bounds_single_axis_move_test<2, float>();
  bounds_bounds_single_axis_move_test<2, double>();
}

TEST(BoundsBounds_SingleAxis_Move, Collision_3D) {

  bounds_bounds_single_axis_move_test<3, float>();
  bounds_bounds_single_axis_move_test<3, double>();
}

TEST(BoundsBounds_SingleAxis_Move, Collision_4D) {

  bounds_bounds_single_axis_move_test<4, float>();
  bounds_bounds_single_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void bounds_bounds_two_axis_move_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      glm::vec<vcl, vct> v1{ 5.f };
      v1[axis1] = 6.125f;
      v1[axis2] = 6.125f;
      glm::vec<vcl, vct> v2{ 5.f };
      v2[axis1] = 3.875f;
      v2[axis2] = 3.875f;
      ntg::bounds<vcl, vct> b1{ -v1, v1 };
      ntg::bounds<vcl, vct> b2{ -v2, v2 };
      while (b1.min[axis1] - b2.max[axis1] < 0.f && b1.min[axis2] - b2.max[axis2] < 0.f)
      {
        EXPECT_TRUE(ntg::collide(b1, b2));
        b1.min[axis1] += isq2 * lambda;
        b1.max[axis1] += isq2 * lambda;
        b1.min[axis2] += isq2 * lambda;
        b1.max[axis2] += isq2 * lambda;
        b2.min[axis1] -= isq2 * lambda;
        b2.max[axis1] -= isq2 * lambda;
        b2.min[axis2] -= isq2 * lambda;
        b2.max[axis2] -= isq2 * lambda;
      }
      EXPECT_FALSE(ntg::collide(b1, b2));
    }
  }
}

TEST(BoundsBounds_TwoAxis_Move, Collision_2D) {

  bounds_bounds_two_axis_move_test<2, float>();
  bounds_bounds_two_axis_move_test<2, double>();
}

TEST(BoundsBounds_TwoAxis_Move, Collision_3D) {

  bounds_bounds_two_axis_move_test<3, float>();
  bounds_bounds_two_axis_move_test<3, double>();
}

TEST(BoundsBounds_TwoAxis_Move, Collision_4D) {

  bounds_bounds_two_axis_move_test<4, float>();
  bounds_bounds_two_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void bounds_radial_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    glm::vec<vcl, vct> v1{ 5.f };
    v1[axis] = 6.125f;
    ntg::bounds<vcl, vct> b{ -v1, v1 };
    ntg::radial<vcl, vct> r{ glm::vec<vcl,vct>{0}, 3.875f };
    while (b.center()[axis] - r.center[axis] <= 10.f)
    {
      EXPECT_TRUE(ntg::collide(b,r));
      b.min[axis] += lambda;
      b.max[axis] += lambda;
      r.center[axis] -= lambda;
    }
    EXPECT_FALSE(ntg::collide(b,r));
  }
}

TEST(BoundsRadial_SingleAxis_Move, Collision_1D) {

  bounds_radial_single_axis_move_test<1, float>();
  bounds_radial_single_axis_move_test<1, double>();
}

TEST(BoundsRadial_SingleAxis_Move, Collision_2D) {

  bounds_radial_single_axis_move_test<2, float>();
  bounds_radial_single_axis_move_test<2, double>();
}

TEST(BoundsRadial_SingleAxis_Move, Collision_3D) {

  bounds_radial_single_axis_move_test<3, float>();
  bounds_radial_single_axis_move_test<3, double>();
}

TEST(BoundsRadial_SingleAxis_Move, Collision_4D) {

  bounds_radial_single_axis_move_test<4, float>();
  bounds_radial_single_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void bounds_radial_two_axis_move_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      glm::vec<vcl, vct> v1{ 5.f };
      v1[axis1] = 6.125f;
      v1[axis2] = 6.125f;
      ntg::bounds<vcl, vct> b{ -v1, v1 };
      ntg::radial<vcl, vct> r{ glm::vec<vcl,vct>{0}, 3.875f };
      while (b.center()[axis1] <= .5f * (isq2 * 3.875f + 6.125f))
      {
        EXPECT_TRUE(ntg::collide(b, r));
        b.min[axis1] += lambda;
        b.min[axis2] += lambda;
        b.max[axis1] += lambda;
        b.max[axis2] += lambda;
        r.center[axis1] -= lambda;
        r.center[axis2] -= lambda;
      }
      EXPECT_FALSE(ntg::collide(b, r));
    }
  }
}

TEST(BoundsRadial_TwoAxis_Move, Collision_2D) {

  bounds_radial_two_axis_move_test<2, float>();
  bounds_radial_two_axis_move_test<2, double>();
}

TEST(BoundsRadial_TwoAxis_Move, Collision_3D) {

  bounds_radial_two_axis_move_test<3, float>();
  bounds_radial_two_axis_move_test<3, double>();
}

TEST(BoundsRadial_TwoAxis_Move, Collision_4D) {

  bounds_radial_two_axis_move_test<4, float>();
  bounds_radial_two_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void radial_point_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    ntg::radial<vcl, vct> r{ glm::vec<vcl,vct>{0},10.f };
    glm::vec<vcl, vct> p{ 0 };
    while (r.center[axis] <= 5.f)
    {
      EXPECT_TRUE(ntg::collide(r,p));
      r.center[axis] += lambda;
      p[axis] -= lambda;
    }
    EXPECT_FALSE(ntg::collide(r,p));
  }
}

TEST(RadialPoint_SingleAxis_Move, Collision_1D) {

  radial_point_single_axis_move_test<1, float>();
  radial_point_single_axis_move_test<1, double>();
}

TEST(RadialPoint_SingleAxis_Move, Collision_2D) {

  radial_point_single_axis_move_test<2, float>();
  radial_point_single_axis_move_test<2, double>();
}

TEST(RadialPoint_SingleAxis_Move, Collision_3D) {

  radial_point_single_axis_move_test<3, float>();
  radial_point_single_axis_move_test<3, double>();
}

TEST(RadialPoint_SingleAxis_Move, Collision_4D) {

  radial_point_single_axis_move_test<4, float>();
  radial_point_single_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void radial_point_two_axis_move_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      ntg::radial<vcl, vct> r{ glm::vec<vcl,vct>{0},10.f };
      glm::vec<vcl, vct> p{ 0 };
      while (r.center[axis1] <= isq2 * 5.f)
      {
        EXPECT_TRUE(ntg::collide(r, p));
        r.center[axis1] += isq2 * lambda;
        r.center[axis2] += isq2 * lambda;
        p[axis1] -= isq2 * lambda;
        p[axis2] -= isq2 * lambda;
      }
      EXPECT_FALSE(ntg::collide(r, p));
    }
  }
}

TEST(RadialPoint_TwoAxis_Move, Collision_2D) {

  radial_point_two_axis_move_test<2, float>();
  radial_point_two_axis_move_test<2, double>();
}

TEST(RadialPoint_TwoAxis_Move, Collision_3D) {

  radial_point_two_axis_move_test<3, float>();
  radial_point_two_axis_move_test<3, double>();
}

TEST(RadialPoint_TwoAxis_Move, Collision_4D) {

  radial_point_two_axis_move_test<4, float>();
  radial_point_two_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void bounds_point_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    ntg::bounds<vcl, vct> b{ glm::vec<vcl,vct>{-10.f},glm::vec<vcl,vct>{10.f} };
    glm::vec<vcl, vct> p{ 0 };
    while (b.center()[axis] <= 5.f)
    {
      EXPECT_TRUE(ntg::collide(b, p));
      b.min[axis] += lambda;
      b.max[axis] += lambda;
      p[axis] -= lambda;
    }
    EXPECT_FALSE(ntg::collide(b, p));
  }
}

TEST(BoundsPoint_SingleAxis_Move, Collision_1D) {

  bounds_point_single_axis_move_test<1, float>();
  bounds_point_single_axis_move_test<1, double>();
}

TEST(BoundsPoint_SingleAxis_Move, Collision_2D) {

  bounds_point_single_axis_move_test<2, float>();
  bounds_point_single_axis_move_test<2, double>();
}

TEST(BoundsPoint_SingleAxis_Move, Collision_3D) {

  bounds_point_single_axis_move_test<3, float>();
  bounds_point_single_axis_move_test<3, double>();
}

TEST(BoundsPoint_SingleAxis_Move, Collision_4D) {

  bounds_point_single_axis_move_test<4, float>();
  bounds_point_single_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void bounds_point_two_axis_move_test()
{
  for (size_t axis1 = 0; axis1 < vcl; ++axis1)
  {
    for (size_t axis2 = axis1 + 1; axis2 < vcl; ++axis2)
    {
      ntg::bounds<vcl, vct> b{ glm::vec<vcl,vct>{-10.f},glm::vec<vcl,vct>{10.f} };
      glm::vec<vcl, vct> p{ 0 };
      while (b.center()[axis1] <= 5.f)
      {
        EXPECT_TRUE(ntg::collide(b, p));
        b.min[axis1] += isq2 * lambda;
        b.min[axis2] += isq2 * lambda;
        b.max[axis1] += isq2 * lambda;
        b.max[axis2] += isq2 * lambda;
        p[axis1] -= isq2 * lambda;
        p[axis2] -= isq2 * lambda;
      }
      EXPECT_FALSE(ntg::collide(b, p));
    }
  }
}

TEST(BoundsPoint_TwoAxis_Move, Collision_2D) {

  bounds_point_two_axis_move_test<2, float>();
  bounds_point_two_axis_move_test<2, double>();
}

TEST(BoundsPoint_TwoAxis_Move, Collision_3D) {

  bounds_point_two_axis_move_test<3, float>();
  bounds_point_two_axis_move_test<3, double>();
}

TEST(BoundsPoint_TwoAxis_Move, Collision_4D) {

  bounds_point_two_axis_move_test<4, float>();
  bounds_point_two_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void highdim_simplex_point_single_axis_move_test()
{
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    ntg::simplex<vcl, vcl+1, vct> s;
    for(size_t i = 0; i < vcl; ++i)
      s.points[i][i] = 10.f;
    glm::vec<vcl, vct> p{ 0 };

    while (p[axis] <= 10.f)
    {
      EXPECT_TRUE(ntg::collide(s, p));
      p[axis] += lambda;
    }
    EXPECT_FALSE(ntg::collide(s, p));
  }
}

TEST(HighDimSimplexPoint_SingleAxis_Move, Collision_1D) {

  highdim_simplex_point_single_axis_move_test<1, float>();
  highdim_simplex_point_single_axis_move_test<1, double>();
}

TEST(HighDimSimplexPoint_SingleAxis_Move, Collision_2D) {

  highdim_simplex_point_single_axis_move_test<2, float>();
  highdim_simplex_point_single_axis_move_test<2, double>();
}

TEST(HighDimSimplexPoint_SingleAxis_Move, Collision_3D) {

  highdim_simplex_point_single_axis_move_test<3, float>();
  highdim_simplex_point_single_axis_move_test<3, double>();
}

TEST(HighDimSimplexPoint_SingleAxis_Move, Collision_4D) {

  highdim_simplex_point_single_axis_move_test<4, float>();
  highdim_simplex_point_single_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void highdim_simplex_point_all_axis_move_test()
{
  ntg::simplex<vcl, vcl+1, vct> s;
  for(size_t i = 0; i < vcl; ++i)
    s.points[i][i] = 10.0;
  glm::vec<vcl, vct> p{ 0 };

  while (p[0] <= 10.0 / vcl)
  {
    EXPECT_TRUE(ntg::collide(s, p));
    for (size_t axis = 0; axis < vcl; ++axis)
    {
      p[axis] += lambda;
    }
  }
  EXPECT_FALSE(ntg::collide(s, p));
}

TEST(HighDimSimplexPoint_AllAxis_Move, Collision_1D) {

  highdim_simplex_point_all_axis_move_test<1, float>();
  highdim_simplex_point_all_axis_move_test<1, double>();
}

TEST(HighDimSimplexPoint_AllAxis_Move, Collision_2D) {

  highdim_simplex_point_all_axis_move_test<2, float>();
  highdim_simplex_point_all_axis_move_test<2, double>();
}

TEST(HighDimSimplexPoint_AllAxis_Move, Collision_3D) {

  highdim_simplex_point_all_axis_move_test<3, float>();
  highdim_simplex_point_all_axis_move_test<3, double>();
}

TEST(HighDimSimplexPoint_AllAxis_Move, Collision_4D) {

  highdim_simplex_point_all_axis_move_test<4, float>();
  highdim_simplex_point_all_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void lowdim_simplex_point_all_axis_move_test()
{
  ntg::simplex<vcl, vcl, vct> s;
  for(size_t i = 0; i < vcl; ++i)
    s.points[i][i] = 10.0;
  glm::vec<vcl, vct> p{ 0 };

  while (p[0] < 10.0 / vcl)
  {
    EXPECT_FALSE(ntg::collide(s, p));
    for (size_t axis = 0; axis < vcl; ++axis)
    {
      p[axis] += lambda;
    }
  }
  for (size_t axis = 0; axis < vcl; ++axis)
    p[axis] = 10.f / vcl;
  EXPECT_TRUE(ntg::collide(s, p));
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    p[axis] += lambda;
  }
  EXPECT_FALSE(ntg::collide(s, p));
}

TEST(LowDimSimplexPoint_AllAxis_Move, Collision_2D) {

  lowdim_simplex_point_all_axis_move_test<2, float>();
  lowdim_simplex_point_all_axis_move_test<2, double>();
}

TEST(LowDimSimplexPoint_AllAxis_Move, Collision_3D) {

  lowdim_simplex_point_all_axis_move_test<3, float>();
  lowdim_simplex_point_all_axis_move_test<3, double>();
}

TEST(LowDimSimplexPoint_AllAxis_Move, Collision_4D) {

  lowdim_simplex_point_all_axis_move_test<4, float>();
  lowdim_simplex_point_all_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void lowdim_simplex_point_planewalk_test()
{
  ntg::simplex<vcl, vcl, vct> s;
  for(size_t i = 0; i < vcl; ++i)
    s.points[i][i] = 10.0;
  glm::vec<vcl-1, vct> barycentric{ 0 };

  while (barycentric[0] <= 1.0 / (vcl-1))
  {
    EXPECT_TRUE(ntg::collide(s, s.toWorld(barycentric)));
    for (size_t axis = 0; axis < vcl-1; ++axis)
    {
      barycentric[axis] += lambda;
    }
  }
  EXPECT_FALSE(ntg::collide(s, s.toWorld(barycentric)));
}

TEST(LowDimSimplexPoint_PlaneWalk, Collision_2D) {

  lowdim_simplex_point_planewalk_test<2, float>();
  lowdim_simplex_point_planewalk_test<2, double>();
}

TEST(LowDimSimplexPoint_PlaneWalk, Collision_3D) {

  lowdim_simplex_point_planewalk_test<3, float>();
  lowdim_simplex_point_planewalk_test<3, double>();
}

TEST(LowDimSimplexPoint_PlaneWalk, Collision_4D) {

  lowdim_simplex_point_planewalk_test<4, float>();
  lowdim_simplex_point_planewalk_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void hyperplane_point_all_axis_move_test()
{
  ntg::hyperplane<vcl, vct> h{glm::vec<vcl,vct>{10.f/vcl}, glm::vec<vcl,vct>{1.f / vcl} };
  glm::vec<vcl, vct> p{ 0 };

  while (p[0] < 10.0 / vcl)
  {
    EXPECT_FALSE(ntg::collide(h, p));
    for (size_t axis = 0; axis < vcl; ++axis)
    {
      p[axis] += lambda;
    }
  }
  for (size_t axis = 0; axis < vcl; ++axis)
    p[axis] = 10.f / vcl;
  EXPECT_TRUE(ntg::collide(h, p));
  for (size_t axis = 0; axis < vcl; ++axis)
  {
    p[axis] += lambda;
  }
  EXPECT_FALSE(ntg::collide(h, p));
}

TEST(HyperplanePoint_AllAxis_Move, Collision_2D) {

  hyperplane_point_all_axis_move_test<2, float>();
  hyperplane_point_all_axis_move_test<2, double>();
}

TEST(HyperplanePoint_AllAxis_Move, Collision_3D) {

  hyperplane_point_all_axis_move_test<3, float>();
  hyperplane_point_all_axis_move_test<3, double>();
}

TEST(HyperplanePoint_AllAxis_Move, Collision_4D) {

  hyperplane_point_all_axis_move_test<4, float>();
  hyperplane_point_all_axis_move_test<4, double>();
}

template <glm::length_t vcl, typename vct>
void hyperplane_point_planewalk_test()
{
  ntg::simplex<vcl,vcl, vct> s;
  for (size_t i = 0; i < vcl; ++i)
    s.points[i][i] = 10.0;
  ntg::hyperplane<vcl, vct> h(s);
  glm::vec<vcl - 1, vct> barycentric{ 0 };

  while (barycentric[0] <= 3.0)
  {
    EXPECT_TRUE(ntg::collide(h, s.toWorld(barycentric)));
    for (size_t axis = 0; axis < vcl - 1; ++axis)
    {
      barycentric[axis] += lambda;
    }
  }
}

TEST(HyperplanePoint_PlaneWalk, Collision_2D) {

  hyperplane_point_planewalk_test<2, float>();
  hyperplane_point_planewalk_test<2, double>();
}

TEST(HyperplanePoint_PlaneWalk, Collision_3D) {

  hyperplane_point_planewalk_test<3, float>();
  hyperplane_point_planewalk_test<3, double>();
}

TEST(HyperplanePoint_PlaneWalk, Collision_4D) {

  hyperplane_point_planewalk_test<4, float>();
  hyperplane_point_planewalk_test<4, double>();
}

glm::vec<2,float> rot(const glm::vec<2, float>& v, float r, glm::vec<2, float> n)
{
  return glm::rotate<float>(v, r);
}

glm::vec<2,double> rot(const glm::vec<2, double>& v, double r, glm::vec<2, double> n)
{
  return glm::rotate<double>(v, r);
}

glm::vec<3,float> rot(const glm::vec<3, float>& v, float r, glm::vec<3, float> n)
{
  return glm::rotate<float>(v, r, n);
}

glm::vec<3,double> rot(const glm::vec<3, double>& v, double r, glm::vec<3, double> n)
{
  return glm::rotate<double>(v, r, n);
}

template <glm::length_t vcl, typename vct>
void hyperplane_ray_single_axis_rotate_test()
{
  for(size_t axis = 0; axis < vcl; ++axis)
  {
    ntg::hyperplane<vcl, vct> h{ glm::vec<vcl,vct>{10.f / vcl}, glm::vec<vcl,vct>{1.f / vcl} };
    glm::vec<vcl, vct> axis_vec{ 0.f };
    axis_vec[axis] = 1;
    axis_vec[(axis + 1) % vcl] = -1;
    ntg::ray<vcl,vct> r{ glm::vec<vcl,vct>{10.f}, glm::vec<vcl,vct>{0} };
    vct angle = -glm::pi<float>() / 2.f;
    
    vct t;
    r.direction = rot((-h.normal), angle, axis_vec);
    EXPECT_FALSE(ntg::collide(r, h, t));
    angle += lambda;

    while (angle < glm::pi<float>() / 2.f)
    {
      r.direction = rot((-h.normal), angle, axis_vec);
      EXPECT_TRUE(ntg::collide(r,h,t));
      auto p = r.origin + t * r.direction;
      EXPECT_TRUE(ntg::collide(p, h));
      angle += lambda;
    }
  }
}

TEST(HyperplaneRay_SingleAxis_Rotate, Collision_2D) {

  hyperplane_ray_single_axis_rotate_test<2, float>();
  hyperplane_ray_single_axis_rotate_test<2, double>();
}

TEST(HyperplaneRay_SingleAxis_Rotate, Collision_3D) {

  hyperplane_ray_single_axis_rotate_test<3, float>();
  hyperplane_ray_single_axis_rotate_test<3, double>();
}