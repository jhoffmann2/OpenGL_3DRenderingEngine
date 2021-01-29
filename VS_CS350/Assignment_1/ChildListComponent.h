#pragma once
#include <vector>


#include "Mesh.h"
#include "ObjectComponent.h"

class ChildListComponent : public ObjectComponent
{
public:
  ChildListComponent();
  ChildListComponent(const std::vector<Mesh>& meshes, const std::vector<std::string>& names);
  ChildListComponent(const std::vector<GameObject*> &children);
  void AddChild(GameObject* object);
  const std::vector<GameObject*> &Children() const;

  // component functions
  void Render() override;
  void Init() override;
  void DebugRender() override;
  void Kill() override;
  void OnObjectDeactivated() override;
  void OnObjectActivated() override;
  void ImGuiEditor() override;
  std::string Name() override;

private:
  std::vector<GameObject *> children_;
};
