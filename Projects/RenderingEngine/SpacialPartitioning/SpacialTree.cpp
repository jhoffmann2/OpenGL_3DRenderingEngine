#include "SpacialTree.h"

#include "GameObjects/GameObject.h"
#include "ntg/collision.inl"
#include "ntg/hit.inl"
#include "imgui.h"

bool SpacialTreeHierarchy::Collide(const glm::vec3& point, ntg::triangle3& hit)
{
  auto& heirarchy = Instance();
  for (auto&& pair : heirarchy.trees_)
  {
    auto modelToWorld = pair.first->GetParentedComponent<TransformComponent>()->GetModelToWorld();
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

bool SpacialTreeHierarchy::Raycast(const ntg::ray3& ray, ntg::hit3& hit)
{
  auto& heirarchy = Instance();
  bool foundHit = false;
  float best_d2 = std::numeric_limits<float>::infinity();
  for (auto&& pair : heirarchy.trees_)
  {
    const auto modelToWorld = pair.first->GetParentedComponent<TransformComponent>()->GetModelToWorld();
    const auto worldToModel = inverse(modelToWorld);
    const ntg::ray3 localRay = worldToModel * ray;
    if (pair.second->Raycast(localRay, hit))
    {
      const auto worldHit = modelToWorld * hit;
      const float d2 = distance2(worldHit.point, ray.origin);
      if(d2 < best_d2)
      {
        best_d2 = d2;
        hit = worldHit;
        foundHit = true;
      }
    }
  }
  return foundHit;
}

bool SpacialTreeHierarchy::Raycast(GameObject* TreeObject, const ntg::ray3& ray, ntg::hit3& hit)
{
  const auto modelToWorld = TreeObject->GetParentedComponent<TransformComponent>()->GetModelToWorld();
  const auto worldToModel = inverse(modelToWorld);
  const ntg::ray3 localRay = worldToModel * ray;
  if (GetTree(TreeObject)->Raycast(localRay, hit))
  {
    hit = modelToWorld * hit;
    return true;
  }
  return false;
}

SpacialTree* SpacialTreeHierarchy::SetTree(GameObject* key, SpacialTree* val)
{
  auto& heirarchy = Instance();
  return heirarchy.trees_[key] = val;
}

void SpacialTreeHierarchy::DeleteTree(GameObject* key)
{
  auto& heirarchy = Instance();
  heirarchy.trees_.erase(key);
}

void SpacialTreeHierarchy::ImguiDraw(const ntg::ray3& mouseRay)
{
  auto& heirarchy = Instance();
  if (ImGui::Begin("Spacial Tree Hierarchy"))
    for (auto &pair : heirarchy.trees_)
      pair.second->ImguiDraw(pair.first, mouseRay);
  ImGui::End();
}

SpacialTreeHierarchy& SpacialTreeHierarchy::Instance()
{
  static SpacialTreeHierarchy heirarchy;
  return heirarchy;
}
