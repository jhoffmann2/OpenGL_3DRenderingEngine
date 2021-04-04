#pragma once
#include "SpacialTree.h"
#include "ntg/bounds.h"

struct OctreeLeaf;
struct OctreeBranch;
struct OctreeNode;

class Octree : public SpacialTree
{
public:
  Octree(const ntg::bounds3 &bounds);
  ~Octree() override;
  void Add(const ntg::triangle3 &tri);
  void Add(const Mesh &mesh);
  [[nodiscard]] OctreeLeaf* MatchLeaf(const glm::vec3& point) const;
  [[nodiscard]] bool Collide(const glm::vec3& point, ntg::triangle3& hit) const override;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const override;
  void ImguiDraw(GameObject* gameobject, const ntg::ray3& mouseRay) override;
  OctreeNode *head_;
};
