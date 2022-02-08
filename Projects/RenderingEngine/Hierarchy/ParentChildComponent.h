#pragma once
#include <unordered_set>

#include "GameObjects/ObjectComponent.h"
#include "Mesh/Mesh.h"

class ParentChildComponent : public ObjectComponent
{
public:
  ParentChildComponent();
  ParentChildComponent(std::unordered_set<ParentChildComponent*> children, ParentChildComponent* parent = nullptr);

  ParentChildComponent(GameObject* parent);
  ParentChildComponent(ParentChildComponent* parent);

  void AddChild(ParentChildComponent* new_child);
  void AddChildrenFromMeshes(
    const std::vector<Mesh>& meshes, 
    const std::vector<std::string>& names
  );
  const std::unordered_set<ParentChildComponent*> &Children() const;

  void SetParent(ParentChildComponent* new_parent);
  [[nodiscard]] ParentChildComponent* GetParent() const;

  // component functions
  void Render() override;
  void Init() override;
  void ForwardRender() override;
  void Kill() override;
  void OnObjectDeactivated() override;
  void OnObjectActivated() override;
  void ImGuiEditor() override;
  std::string Name() override;

private:
  std::unordered_set<ParentChildComponent*> children_;
  ParentChildComponent*parent_ = nullptr;
};