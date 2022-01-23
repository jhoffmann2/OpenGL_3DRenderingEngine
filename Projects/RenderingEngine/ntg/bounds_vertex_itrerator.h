#pragma once

#include "bounds.h"
#include "glm/vec3.hpp"
namespace ntg
{
  using namespace glm;
  template<length_t vcl = 3, typename vct = float>
  class bounds_vertex_iterator
  {
  public:
    bounds_vertex_iterator() = default;
    bounds_vertex_iterator(const bounds<vcl, vct>& bounds);
    bounds_vertex_iterator& operator++();
    bounds_vertex_iterator operator++(int) const;
    vec<vcl,vct> operator*() const;
  private:
    bool valid_index();
    constexpr size_t maxLength = 1 << vcl;
    vec<vcl,vct> half_size;
    vec<vcl,vct> center;
    size_t index = maxLength;
  };
}
