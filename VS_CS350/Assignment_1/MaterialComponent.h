#pragma once
#include "MaterialHandle.h"
#include "ObjectComponent.h"

class MaterialComponent : public ObjectComponent, public MaterialHandle
{
public:

  MaterialComponent(size_t index);
  MaterialComponent(const MaterialHandle &handle);
  void PreRender() override;

  void ImGuiEditor() override;
  std::string Name() override;
};

