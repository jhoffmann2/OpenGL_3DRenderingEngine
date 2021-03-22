#pragma once
#include <array>
#include <vector>


#include "SpacialTree.h"
#include "ntg/bounds.h"
#include "ntg/ray.h"
#include "ntg/simplex.h"

struct Mesh;
struct Leaf;
struct Branch;
class Octree;

struct Node
{
  Node(bool isLeaf, const ntg::bounds3& bounds, Octree* tree);
  Node(bool isLeaf, const ntg::bounds3& bounds, Branch* parent);
  Node(bool isLeaf, Branch* parent, unsigned char octant);
  [[nodiscard]] Leaf *ToLeaf();
  [[nodiscard]] Branch *ToBranch();
  [[nodiscard]] const Leaf *ToLeaf() const;
  [[nodiscard]] const Branch*ToBranch() const;
  bool isLeaf_;
  ntg::bounds3 bounds_;
  Branch *parent_ = nullptr;
  Octree* tree_;
};

struct Leaf : Node
{
  Leaf(const ntg::bounds3& bounds, Octree* tree);
  Leaf(const ntg::bounds3& bounds, Branch* parent);
  Leaf(Branch* parent, unsigned char octant);
  void Add(const ntg::triangle3& tri);
  Branch *Split();
  void ImguiDraw(const glm::mat4& transform);
  void Render() const;
  std::vector<ntg::triangle3> triangles_;
};

struct Branch : Node
{
  Branch(const ntg::bounds3& bounds, Octree* tree);
  Branch(const ntg::bounds3& bounds, Branch *parent);
  Branch(Branch *parent, unsigned char octant);
  [[nodiscard]] Node*& MatchChild(const glm::vec3& point);
  [[nodiscard]] Node* MatchChild(const glm::vec3& point) const;
  [[nodiscard]] Leaf* MatchLeaf(const glm::vec3& point);
  [[nodiscard]] unsigned char octant(const glm::vec3& point) const;
  void Add(const ntg::triangle3& tri);
  void ForceAdd(const ntg::triangle3& tri);
  void ImguiDraw(const glm::mat4& transform);
  void Render() const;

  std::array<Node*, 8> children_;
};

class Octree : public SpacialTree
{
public:
  Octree(const ntg::bounds3 &bounds);
  void Add(const ntg::triangle3 &tri) override;
  void Add(const Mesh &mesh) override;
  [[nodiscard]] Leaf* MatchLeaf(const glm::vec3& point) const;
  [[nodiscard]] bool Collide(const glm::vec3& point, ntg::triangle3& hit) const override;
  void ImguiDraw(TransformComponent* transform) override;
  Node *head_;
};
