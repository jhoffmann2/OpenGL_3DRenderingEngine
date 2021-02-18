#pragma once
#include "ObjectComponent.h"
#include "ntg/bounds.h"
#include <typeinfo>

#include "ntg/collision.h"

class BoxVolumeComponent : public ObjectComponent
{
public:
  BoxVolumeComponent(const ntg::bounds3& volume) : volume_(volume)
  {}

  std::string Name() override
  {
    return std::string("Box Volume Component");
  }

  template<typename geometry>
  bool collide(const geometry& other)
  {
    // collide both volumes into world space
    return ntg::collide(volume_, other);
  }
  ntg::bounds3 volume_;
};


