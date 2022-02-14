#pragma once
#include <glm/mat4x4.hpp>
#include <ntg/bounds.h>
#include <vector>

#include "GameObjects/ObjectComponent.h"
#include "LightHandle.h"

class LightComponent : public ObjectComponent, public LightHandle
{
public:
  LightComponent();

  void PreRender() override;
  void OnObjectActivated() override;
  void OnObjectDeactivated() override;
  LightComponent(size_t index);

  void ImGuiEditor() override;
  std::string Name() override;

  void RenderShadowMap(const std::vector<GameObject*>& objects,
                       const ntg::bounds3& sceneBounds);
private:
  bool pointToTarget_ = true;
  glm::vec3 target_ = {};
};
