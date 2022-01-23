#pragma once
#include "bounds_vertex_itrerator.h"
#include "bounds.h"
namespace ntg
{
  template <length_t vcl, typename vct>
  bounds_vertex_iterator<vcl, vct>::bounds_vertex_iterator(const bounds<vcl, vct>& bounds) :
    half_size(bounds.half_size()), center(bounds.center()), index(0)
  {}

  template <length_t vcl, typename vct>
  bounds_vertex_iterator<vcl, vct>& bounds_vertex_iterator<vcl, vct>::operator++()
  {
    do ++index; while (index < maxLength && !valid_index());
    return *this;
  }

  template <length_t vcl, typename vct>
  bounds_vertex_iterator<vcl, vct> bounds_vertex_iterator<vcl, vct>::operator++(int) const
  {
    return ++bounds_vertex_iterator<vcl, vct>(*this);
  }

  template <length_t vcl, typename vct>
  vec<vcl, vct> bounds_vertex_iterator<vcl, vct>::operator*() const
  {
    assert(index < maxLength);
    vec<vcl, vct> output = center;

    int bitmask = 1;
    for (size_t i = 0; i < vcl; ++i)
    {
      output[i] += (bitmask & index) ? half_size[i] : -half_size[i];
      bitmask <<= 1;
    }
    return output;
  }

  template <length_t vcl, typename vct>
  bool bounds_vertex_iterator<vcl, vct>::valid_index()
  {
    int bitmask = 1;
    for (size_t i = 0; i < vcl; ++i)
    {
      if (bitmask & index && half_size[i] == 0)
        return false;
      bitmask <<= 1;
    }
    return true;
  }
}