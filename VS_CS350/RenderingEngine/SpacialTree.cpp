#include "SpacialTree.h"

#include "imgui.h"
#include "ntg/collision.inl"

bool SpacialTreeHierarchy::Collide(const glm::vec3& point, ntg::triangle3& hit)
{
  auto& heirarchy = Instance();
  for (auto&& pair : heirarchy.trees_)
  {
    auto modelToWorld = pair.first->GetModelToWorld();
    auto worldToModel = inverse(modelToWorld);
    ntg::vec3 localPoint = worldToModel * glm::vec4(point, 1);
    if (pair.second->Collide(localPoint, hit))
    {
      hit = modelToWorld * hit;
      return true;
    }
  }
  return false;
}

SpacialTree *SpacialTreeHierarchy::GetTree(TransformComponent* key)
{
  auto &heirarchy = Instance();
  return heirarchy.trees_[key];
}

SpacialTree* SpacialTreeHierarchy::SetTree(TransformComponent* key, SpacialTree* val)
{
  auto& heirarchy = Instance();
  return heirarchy.trees_[key] = val;
}

void SpacialTreeHierarchy::DeleteTree(TransformComponent* key)
{
  auto& heirarchy = Instance();
  heirarchy.trees_.erase(key);
}

void SpacialTreeHierarchy::ImguiDraw()
{
  auto& heirarchy = Instance();
  if (ImGui::Begin("Spacial Tree Hierarchy"))
    for (auto &pair : heirarchy.trees_)
      pair.second->ImguiDraw(pair.first);
  ImGui::End();
}

SpacialTreeHierarchy& SpacialTreeHierarchy::Instance()
{
  static SpacialTreeHierarchy heirarchy;
  return heirarchy;
}
