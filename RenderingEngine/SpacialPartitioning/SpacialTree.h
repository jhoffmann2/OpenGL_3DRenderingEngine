#pragma once
#include <unordered_map>

#include "Transform/TransformComponent.h"
#include "ntg/hit.h"
#include "ntg/ray.h"
#include "ntg/simplex.h"

struct Mesh;

class SpacialTree
{
public:
  virtual ~SpacialTree() = default;
  [[nodiscard]] virtual bool Collide(const glm::vec3& point, ntg::triangle3& hit) const = 0;
  [[nodiscard]] virtual bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const = 0;
  virtual void ImguiDraw(GameObject* transform, const ntg::ray3& mouseRay) = 0;
};

class SpacialTreeHierarchy
{
public:
  [[nodiscard]] static bool Collide(const glm::vec3& point, ntg::triangle3& hit);
  [[nodiscard]] static bool Raycast(const ntg::ray3& ray, ntg::hit3& hit);
  [[nodiscard]] static bool Raycast(GameObject *TreeObject, const ntg::ray3& ray, ntg::hit3& hit);
  template<typename TreeType = SpacialTree>
  [[nodiscard]] static TreeType* GetTree(GameObject* key);
  static SpacialTree *SetTree(GameObject* key, SpacialTree* val);
  static void DeleteTree(GameObject* key);
  static void ImguiDraw(const ntg::ray3& mouseRay);
  SpacialTreeHierarchy(SpacialTreeHierarchy&) = delete;
private:
  static SpacialTreeHierarchy& Instance();
  SpacialTreeHierarchy() = default;
  std::unordered_map<GameObject *, SpacialTree *> trees_;
};

template <typename TreeType>
TreeType* SpacialTreeHierarchy::GetTree(GameObject* key)
{
    auto& heirarchy = Instance();
    return static_cast<TreeType*>(heirarchy.trees_[key]);
}
