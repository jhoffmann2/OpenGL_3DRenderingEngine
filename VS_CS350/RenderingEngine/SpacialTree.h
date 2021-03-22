#pragma once
#include <unordered_map>


#include "TransformComponent.h"
#include "ntg/ray.h"
#include "ntg/simplex.h"

struct Mesh;

class SpacialTree
{
public:

  virtual void Add(const ntg::triangle3 &tri) = 0;
  virtual void Add(const Mesh &mesh) = 0;
  [[nodiscard]] virtual bool Collide(const glm::vec3& point, ntg::triangle3& hit) const = 0;
  virtual void ImguiDraw(TransformComponent* transform) = 0;
};

class SpacialTreeHierarchy
{
public:
  [[nodiscard]] static bool Collide(const glm::vec3& point, ntg::triangle3& hit);
  [[nodiscard]] static SpacialTree *GetTree(TransformComponent* key);
  static SpacialTree *SetTree(TransformComponent* key, SpacialTree* val);
  static void DeleteTree(TransformComponent* key);
  static void ImguiDraw();
  SpacialTreeHierarchy(SpacialTreeHierarchy&) = delete;
private:
  static SpacialTreeHierarchy& Instance();
  SpacialTreeHierarchy() = default;
  std::unordered_map<TransformComponent *, SpacialTree *> trees_;
};