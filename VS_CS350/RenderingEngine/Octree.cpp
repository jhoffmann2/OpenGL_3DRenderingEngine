#include "Octree.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.inl>
#include <glm/gtx/color_space.hpp>

#include "DebugDraw.h"
#include "imgui.h"
#include "Mesh.h"
#include "ntg/collision.inl"
#include "ntg/bounds.inl"
#include "ntg/hyperplane.inl"
#include "ntg/simplex.inl"
#include "GameObject.h"
#include "VertexGlobalSystem.h"

Node::Node(bool isLeaf, const ntg::bounds3& bounds, Octree* tree) :
  isLeaf_(isLeaf), bounds_(bounds), parent_(nullptr), tree_(tree)
{
}

Node::Node(bool isLeaf, const ntg::bounds3& bounds, Branch* parent) :
  isLeaf_(isLeaf), bounds_(bounds), parent_(parent_), tree_(parent->tree_)
{
}

Node::Node(bool isLeaf, Branch* parent, size_t octant) :
  isLeaf_(isLeaf), parent_(parent), tree_(parent->tree_)
{
  static const glm::vec3 directions[8] = {
    {-1,-1,-1},
    {1,-1,-1},
    {-1,1,-1},
    {1,1,-1},
    {-1, -1, 1},
    {1, -1, 1},
    {-1, 1, 1},
    {1, 1, 1},
  };
  auto p0 = parent_->bounds_.center();
  glm::vec3 diagnal = parent_->bounds_.half_size();
  const glm::vec3 d = directions[octant];
  diagnal.x *= d.x;
  diagnal.y *= d.y;
  diagnal.z *= d.z;
  bounds_ = ntg::bounds3::FromTwoCorners(p0, p0 + diagnal);
}

Leaf* Node::ToLeaf()
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<Leaf*>(this);
}

Branch* Node::ToBranch()
{
  if(isLeaf_)
    return nullptr;
  return static_cast<Branch*>(this);
}

const Leaf* Node::ToLeaf() const
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<const Leaf*>(this);
}

const Branch* Node::ToBranch() const
{
  if (isLeaf_)
    return nullptr;
  return static_cast<const Branch*>(this);
}

Leaf::Leaf(const ntg::bounds3& bounds, Octree* tree) :
  Node(true, bounds, tree)
{
}

Leaf::Leaf(const ntg::bounds3& bounds, Branch* parent) :
  Node(true, bounds, parent)
{
}

Leaf::Leaf(Branch* parent, size_t octant) :
  Node(true, parent, octant)
{
}

void Leaf::Add(const ntg::triangle3& tri)
{
  triangles_.push_back(tri);
  float d = 1.f / 3.f;
  /*assert(tree_->MatchLeaf(tri.toWorld({ d, d, d })) == this);
  assert(tree_->MatchLeaf(tri.toWorld({ d, d, d })) == this);
  assert(tree_->MatchLeaf(tri.toWorld({ d, d, d })) == this);*/
  if(triangles_.size() >= 5000)
    Split();
}

Branch* Leaf::Split()
{
  Branch* branch;
  if(parent_)
  {
    branch = new Branch(bounds_, parent_);
    parent_->MatchChild(bounds_.center()) = branch;
  }
  else
  {
    branch = new Branch(bounds_, tree_);
    tree_->head_ = branch;
  }
  for (ntg::triangle3 tri : triangles_)
    branch->Add(tri);
  delete this;
  return branch;
}

Node*& Branch::MatchChild(const glm::vec3& point)
{
  return children_[octant(point)];
}

Node* Branch::MatchChild(const glm::vec3& point) const
{
  return children_[octant(point)];
}

Leaf* Branch::MatchLeaf(const glm::vec3& point)
{
  const size_t oct = octant(point);
  Node *&child = children_[oct];

  if(!child)
    return (child = new Leaf(this, oct))->ToLeaf();
  if(child->isLeaf_)
    return child->ToLeaf();
  return child->ToBranch()->MatchLeaf(point);
}

size_t Branch::octant(const glm::vec3& point) const
{
  const glm::vec3 v = point - bounds_.center();
  return ((v[0] >= -1e-6)) |
    ((v[1] >= -1e-6) << 1) |
    ((v[2] >= -1e-6) << 2);
}

size_t firstSetBit(size_t v)
{
  auto&vl = reinterpret_cast<long long&>(v);
  long long out = vl & -vl;
  return reinterpret_cast<size_t &>(out);
}

glm::vec3 getSplitPlaneNormal(size_t octant1, size_t octant2)
{
  switch (firstSetBit(octant1 ^ octant2))
  {
  case 1: // yz plane
    return { 1, 0, 0 };
  case 2: // xz plane
    return { 0, 1, 0 };
  case 4: // xy plane
    return { 0, 0, 1 };
  }
  throw std::out_of_range("Octants must not match");
}

void Branch::Add(const ntg::triangle3& tri)
{
  std::vector<ntg::triangle3> frontSplit;
  frontSplit.reserve(2);
  std::vector<ntg::triangle3> backSplit;
  frontSplit.reserve(2);

  size_t oct0 = octant(tri.points[0]);
  const size_t oct1 = octant(tri.points[1]);
  const size_t oct2 = octant(tri.points[2]);

  if(oct0 != oct1)
  {
    const ntg::hyperplane3 p(bounds_.center(), getSplitPlaneNormal(oct0, oct1));
    if(split(tri, p, frontSplit, backSplit))
    {
      for (const ntg::triangle3 &new_tri : frontSplit)
        Add(new_tri);
      for (const ntg::triangle3 &new_tri : backSplit)
        Add(new_tri);
      return;
    }
    if(oct1 == oct2)
      oct0 = oct1;
  }
  else if (oct0 != oct2)
  {
    const ntg::hyperplane3 p(bounds_.center(), getSplitPlaneNormal(oct0, oct2));
    if (split(tri, p, frontSplit, backSplit))
    {
      for (const ntg::triangle3& new_tri : frontSplit)
        Add(new_tri);
      for (const ntg::triangle3& new_tri : backSplit)
        Add(new_tri);
      return;
    }
    if (oct1 == oct2)
      oct0 = oct1;
  }
  

  // if you've made it this far, all three points are in the same octant yay!
  if(children_[oct0] == nullptr)
  {
    Leaf *l = new Leaf(this, oct0);
    children_[oct0] = l;
    l->Add(tri);
    return;
  }
  if(children_[oct0]->isLeaf_)
  {
    children_[oct0]->ToLeaf()->Add(tri);
    return;
  }
  children_[oct0]->ToBranch()->Add(tri);
}

Branch::Branch(const ntg::bounds3& bounds, Octree* tree) :
  Node(false, bounds, tree)
{
  children_.fill(nullptr);
}

Branch::Branch(const ntg::bounds3& bounds, Branch* parent):
  Node(false, bounds, parent)
{
  children_.fill(nullptr);
}

Branch::Branch(Branch* parent, size_t octant) :
  Node(false, parent, octant)
{
  children_.fill(nullptr);
}

Octree::Octree(const ntg::bounds3& bounds) :
head_(new Leaf(bounds, this))
{
}

void Octree::Add(const ntg::triangle3& tri)
{
  if (head_->isLeaf_)
  {
    head_->ToLeaf()->Add(tri);
    return;
  }
  head_->ToBranch()->Add(tri);
}

void Octree::Add(const Mesh& mesh)
{
  for (const glm::uvec3& face : mesh.faces)
  {
    Add(ntg::triangle3{
      mesh.vertices[face[0]],
      mesh.vertices[face[1]],
      mesh.vertices[face[2]],
    });
  }
}

Leaf* Octree::MatchLeaf(const glm::vec3& point) const
{
  if (head_->isLeaf_)
    return head_->ToLeaf();
  return head_->ToBranch()->MatchLeaf(point);
}

bool Octree::Collide(const glm::vec3& point, ntg::triangle3& hit) const
{
  return false;
}

void Octree::ImguiDraw(TransformComponent* transform)
{
  srand(0);
  auto *gameobject = transform->GetGameObject();
  ImGui::PushID(gameobject->ID());
  ImGui::Indent(10);
  if (ImGui::CollapsingHeader(gameobject->Name().c_str()))
  {
    const glm::mat4x4 t = transform->GetModelToWorld();
    VertexGlobalSystem::SetModelToWorld(t);
    if (head_->isLeaf_)
      head_->ToLeaf()->ImguiDraw(t);
    else
      head_->ToBranch()->ImguiDraw(t);
  }
  ImGui::Unindent(10);
  ImGui::PopID();
}

void Branch::ImguiDraw(const glm::mat4& transform)
{
  const float hue = glm::linearRand(0.f, 360.f);
  DebugDraw::SetColor(rgbColor(glm::vec3{ hue, 1.f, 1.f }));

  ImGui::Indent(10);
  const ntg::bounds3 worldbounds = transform * bounds_;
  ImGui::Text("Bounds: [%.2f, %.2f, %.2f], [%.2f, %.2f, %.2f]",
    worldbounds.min.x,
    worldbounds.min.y,
    worldbounds.min.z,
    worldbounds.max.x,
    worldbounds.max.y,
    worldbounds.max.z
  );
  for(size_t i = 0; i < children_.size(); ++i)
  {
    if(children_[i])
    {
      ImGui::PushID(static_cast<int>(i));
      
      if(children_[i]->isLeaf_)
      {
        if (ImGui::CollapsingHeader("Leaf"))
          children_[i]->ToLeaf()->ImguiDraw(transform);
        else
          children_[i]->ToLeaf()->Render();
      }
      else
      {
        if (ImGui::CollapsingHeader("Branch"))
          children_[i]->ToBranch()->ImguiDraw(transform);
        else
          children_[i]->ToBranch()->Render();
      }

      ImGui::PopID();
    }
  }
  ImGui::Unindent(10);
}

void Leaf::ImguiDraw(const glm::mat4& transform)
{
  const float hue = glm::linearRand(0.f, 360.f);
  DebugDraw::SetColor(rgbColor(glm::vec3{ hue, 1.f, 1.f }));

  ImGui::Indent(10);
  const ntg::bounds3 worldbounds = transform * bounds_;
  Render();
  ImGui::Text("Bounds: [%.2f, %.2f, %.2f], [%.2f, %.2f, %.2f]",
    worldbounds.min.x,
    worldbounds.min.y,
    worldbounds.min.z,
    worldbounds.max.x,
    worldbounds.max.y,
    worldbounds.max.z
  );
  if(ImGui::Button("Split"))
  {
    Split();
    return;
  }
  char buffer[32]{0};
  sprintf_s(buffer, 32, "Triangles [%llu]", triangles_.size());
  if (ImGui::CollapsingHeader(buffer))
  {
    for(const ntg::triangle3 &tri : triangles_)
    {
      const ntg::triangle3 worldTri = transform * tri;
      ImGui::Text("(%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)",
        worldTri.points[0].x,
        worldTri.points[0].y,
        worldTri.points[0].z,

        worldTri.points[1].x,
        worldTri.points[1].y,
        worldTri.points[1].z,

        worldTri.points[2].x,
        worldTri.points[2].y,
        worldTri.points[2].z
      );
    }
  }
  ImGui::Unindent(10);
}

void Leaf::Render() const
{
  DebugDraw::DrawWireframe(triangles_);
}

void Branch::Render() const
{
  for (Node *child : children_)
  {
    if (child)
    {
      if (child->isLeaf_)
        child->ToLeaf()->Render();
      else
        child->ToBranch()->Render();
    }
  }
}
