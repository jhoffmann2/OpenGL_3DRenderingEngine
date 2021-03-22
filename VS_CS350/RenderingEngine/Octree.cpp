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

Node::Node(bool isLeaf, Branch* parent, unsigned char octant) :
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

Leaf::Leaf(Branch* parent, unsigned char octant) :
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
  const signed oct = octant(point);
  Node *&child = children_[oct];

  if(!child)
    return (child = new Leaf(this, oct))->ToLeaf();
  if(child->isLeaf_)
    return child->ToLeaf();
  return child->ToBranch()->MatchLeaf(point);
}

unsigned char Branch::octant(const glm::vec3& point) const
{
  const glm::vec3 v = point - bounds_.center();
  return ((v[0] >= -1e-6)) |
    ((v[1] >= -1e-6) << 1) |
    ((v[2] >= -1e-6) << 2);
}

unsigned char firstSetBit(unsigned char v)
{
  auto&vl = reinterpret_cast<char&>(v);
  char out = vl & -vl;
  return reinterpret_cast<unsigned char&>(out);
}

bool getSplitPlaneNormal(unsigned char o1, unsigned char o2, unsigned char o3, size_t direction, glm::vec3 &out_normal)
{
  const unsigned char v = ~((o1 & o2 & o3) | ~(o1 | o2 | o3)) & (1 << direction);
  switch (v)
  {
  case 1: // yz plane
    out_normal = { 1, 0, 0 };
    return true;
  case 2: // xz plane
    out_normal = { 0, 1, 0 };
    return true;
  case 4: // xy plane
    out_normal = { 0, 0, 1 };
    return true;
  }
  return false;
}

void Branch::Add(const ntg::triangle3& tri)
{
  std::vector<ntg::triangle3> frontSplit;
  frontSplit.reserve(2);
  std::vector<ntg::triangle3> backSplit;
  frontSplit.reserve(2);

  unsigned char oct0 = octant(tri.points[0]);
  const unsigned char oct1 = octant(tri.points[1]);
  const unsigned char oct2 = octant(tri.points[2]);

  std::vector<ntg::triangle3> triangles;
  triangles.push_back(tri);

  for(size_t direction = 0; direction < 3; ++direction)
  {
    std::vector<ntg::triangle3> split_triangles;
    glm::vec3 n;
    if(getSplitPlaneNormal(oct0, oct1, oct2, direction, n))
    {
      for (const ntg::triangle3 &triangle : triangles)
      {
        if (split(triangle, { bounds_.center(),n }, frontSplit, backSplit))
        {
          for (const ntg::triangle3& new_tri : frontSplit)
              split_triangles.push_back(new_tri);
          for (const ntg::triangle3& new_tri : backSplit)
              split_triangles.push_back(new_tri);
        }
        else
          split_triangles.push_back(triangle);
      }
      triangles = std::move(split_triangles);
    }
  }
  for (const ntg::triangle3& triangle : triangles)
      ForceAdd(triangle);
}

void Branch::ForceAdd(const ntg::triangle3& tri)
{
  const unsigned char oct = octant(tri.toWorld({1.f/3.f, 1.f / 3.f, 1.f / 3.f}));
  if (children_[oct] == nullptr)
  {
    Leaf* l = new Leaf(this, oct);
    children_[oct] = l;
    l->Add(tri);
    return;
  }
  if (children_[oct]->isLeaf_)
  {
    children_[oct]->ToLeaf()->Add(tri);
    return;
  }
  children_[oct]->ToBranch()->Add(tri);
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

Branch::Branch(Branch* parent, unsigned char octant) :
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
  DebugDraw::DrawAABB(bounds_, GL_LINE);

  ImGui::Indent(10);
  const ntg::bounds3 worldbounds = transform * bounds_;

  ImGui::PushStyleColor(ImGuiCol_Text, {1.f, 1.f, 1.f, 1.f});
  ImGui::Text("Bounds: [%.2f, %.2f, %.2f], [%.2f, %.2f, %.2f]",
    worldbounds.min.x,
    worldbounds.min.y,
    worldbounds.min.z,
    worldbounds.max.x,
    worldbounds.max.y,
    worldbounds.max.z
  );
  ImGui::PopStyleColor();

  for(size_t i = 0; i < children_.size(); ++i)
  {
    if(children_[i])
    {
      ImGui::PushID(static_cast<int>(i));
      
      const glm::vec3 color = rgbColor(
          linearRand(
            glm::vec3{ 0.f, 0.5f, 0.5f }, 
            glm::vec3{ 360.f, 1.f, 1.f }
          )
        );
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{color.r, color.g, color.b, 1.f});
      DebugDraw::SetColor(glm::vec4{ color, 1.f });

      if(children_[i]->isLeaf_)
      {
        if (ImGui::CollapsingHeader("Leaf"))
          children_[i]->ToLeaf()->ImguiDraw(transform);
        else
        {
          if(ImGui::IsItemHovered())
          {
            const ImGuiStyle& style = ImGui::GetStyle();
            auto c = style.Colors[ImGuiCol_HeaderHovered];
            DebugDraw::SetColor(glm::vec4{ 1.f, 1.f, 1.f, 1.f });
          }
          DebugDraw::DrawAABB(children_[i]->bounds_, GL_LINE);
          children_[i]->ToLeaf()->Render(); 
        }
      }
      else
      {
        if (ImGui::CollapsingHeader("Branch"))
          children_[i]->ToBranch()->ImguiDraw(transform);
        else
        {
          if (ImGui::IsItemHovered())
          {
            const ImGuiStyle& style = ImGui::GetStyle();
            auto c = style.Colors[ImGuiCol_HeaderHovered];
            DebugDraw::SetColor(glm::vec4{ 1.f, 1.f, 1.f, 1.f });
          }
          DebugDraw::DrawAABB(children_[i]->bounds_, GL_LINE);
          children_[i]->ToBranch()->Render();
        }
      }
      ImGui::PopStyleColor();

      ImGui::PopID();
    }
  }
  ImGui::Unindent(10);
}

void Leaf::ImguiDraw(const glm::mat4& transform)
{
  DebugDraw::DrawAABB(bounds_, GL_LINE);

  ImGui::Indent(10);
  const ntg::bounds3 worldbounds = transform * bounds_;
  Render();

  ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 1.f });
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
  ImGui::PopStyleColor();
}

void Leaf::Render() const
{
  DebugDraw::DrawTriangleList(triangles_, GL_LINE);
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
