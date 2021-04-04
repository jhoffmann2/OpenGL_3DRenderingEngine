#pragma once
#include "SpacialTree.h"

struct BspLeaf;
struct BspBranch;
struct BspNode;

class BspTree : public SpacialTree
{
public:
  BspTree(const std::vector<Mesh>& meshes);
  ~BspTree() override;
  [[nodiscard]] bool Collide(const glm::vec3& point, ntg::triangle3& hit) const override;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const override;
  void ImguiDraw(GameObject* gameobject, const ntg::ray3& mouseRay) override;
  BspNode *head_;
};

