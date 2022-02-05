#include "Octree.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.inl>
#include <glm/gtx/color_space.hpp>

#include "GameObjects/GameObject.h"
#include "Mesh/Mesh.h"
#include "Rendering/DebugDraw.h"
#include "Transform/VertexGlobalSystem.h"
#include "ntg/bounds.inl"
#include "ntg/collision.inl"
#include "ntg/hyperplane.inl"
#include "ntg/simplex.inl"
#include "imgui.h"

struct OctreeNode
{
  OctreeNode(bool isLeaf, const ntg::bounds3& bounds, Octree* tree);
  OctreeNode(bool isLeaf, const ntg::bounds3& bounds, OctreeBranch* parent);
  OctreeNode(bool isLeaf, OctreeBranch* parent, unsigned char octant);
  [[nodiscard]] OctreeLeaf* ToLeaf();
  [[nodiscard]] OctreeBranch* ToBranch();
  [[nodiscard]] const OctreeLeaf* ToLeaf() const;
  [[nodiscard]] const OctreeBranch* ToBranch() const;
  [[nodiscard]] unsigned char octant(const glm::vec3& point) const;
  [[nodiscard]] size_t CalculateSplitSize(const ntg::triangle3& tri) const;
  void DestroyRecursive();
  bool isLeaf_;
  ntg::bounds3 bounds_;
  OctreeBranch* parent_ = nullptr;
  Octree* tree_ = nullptr;
};

struct OctreeLeaf : OctreeNode
{
  OctreeLeaf(const ntg::bounds3& bounds, Octree* tree);
  OctreeLeaf(const ntg::bounds3& bounds, OctreeBranch* parent);
  OctreeLeaf(OctreeBranch* parent, unsigned char octant);
  OctreeBranch* Split();
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const;
  void Add(const ntg::triangle3& tri, bool noSplit = false);
  void ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay);
  void Render() const;
  std::vector<ntg::triangle3> triangles_;
  size_t splitSize_ = 0;
};

struct OctreeBranch : OctreeNode
{
  OctreeBranch(const ntg::bounds3& bounds, Octree* tree);
  OctreeBranch(const ntg::bounds3& bounds, OctreeBranch* parent);
  OctreeBranch(OctreeBranch* parent, unsigned char octant);
  [[nodiscard]] OctreeNode*& MatchChild(OctreeNode* node);
  [[nodiscard]] bool HasChild(OctreeNode* node);
  [[nodiscard]] OctreeNode*& MatchChild(const glm::vec3& point);
  [[nodiscard]] OctreeNode* MatchChild(const glm::vec3& point) const;
  [[nodiscard]] OctreeLeaf* MatchLeaf(const glm::vec3& point);
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const;
  void Add(const ntg::triangle3& tri, bool noSplit = false);
  void ForceAdd(const ntg::triangle3& tri, bool noSplit = false);
  void ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay);
  void Render() const;

  std::array<OctreeNode*, 8> children_;
};

bool getSplitPlaneNormal(unsigned char o1, unsigned char o2, unsigned char o3, size_t direction, glm::vec3& out_normal)
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

OctreeNode::OctreeNode(bool isLeaf, const ntg::bounds3& bounds, Octree* tree) :
  isLeaf_(isLeaf), bounds_(bounds), parent_(nullptr), tree_(tree)
{
}

OctreeNode::OctreeNode(bool isLeaf, const ntg::bounds3& bounds, OctreeBranch* parent) :
  isLeaf_(isLeaf), bounds_(bounds), parent_(parent), tree_(parent->tree_)
{
}

OctreeNode::OctreeNode(bool isLeaf, OctreeBranch* parent, unsigned char octant) :
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

OctreeLeaf* OctreeNode::ToLeaf()
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<OctreeLeaf*>(this);
}

OctreeBranch* OctreeNode::ToBranch()
{
  if(isLeaf_)
    return nullptr;
  return static_cast<OctreeBranch*>(this);
}

const OctreeLeaf* OctreeNode::ToLeaf() const
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<const OctreeLeaf*>(this);
}

const OctreeBranch* OctreeNode::ToBranch() const
{
  if (isLeaf_)
    return nullptr;
  return static_cast<const OctreeBranch*>(this);
}

OctreeLeaf::OctreeLeaf(const ntg::bounds3& bounds, Octree* tree) :
  OctreeNode(true, bounds, tree)
{
}

OctreeLeaf::OctreeLeaf(const ntg::bounds3& bounds, OctreeBranch* parent) :
  OctreeNode(true, bounds, parent)
{
}

OctreeLeaf::OctreeLeaf(OctreeBranch* parent, unsigned char octant) :
  OctreeNode(true, parent, octant)
{
}

void OctreeLeaf::Add(const ntg::triangle3& tri, bool noSplit)
{
  triangles_.push_back(tri);
  splitSize_ += CalculateSplitSize(tri);

  if(noSplit) 
    return;

  if(triangles_.size() < 500)
    return;

  if(splitSize_ > 1.5f * triangles_.size())
    return;

  glm::vec3 b_size = bounds_.size();
  float minSize = glm::min(glm::min(b_size.x, b_size.y), b_size.z);

  if(minSize <= 1e-6)
    return;

  Split();
}

size_t OctreeNode::CalculateSplitSize(const ntg::triangle3& tri) const
{
  unsigned char oct0 = octant(tri.points[0]);
  const unsigned char oct1 = octant(tri.points[1]);
  const unsigned char oct2 = octant(tri.points[2]);

  std::vector<ntg::triangle3> triangles;
  triangles.push_back(tri);

  std::vector<ntg::triangle3> frontSplit;
  frontSplit.reserve(4);
  std::vector<ntg::triangle3> backSplit;
  frontSplit.reserve(4);

  for (size_t direction = 0; direction < 3; ++direction)
  {
    std::vector<ntg::triangle3> split_triangles;
    glm::vec3 n;
    if (getSplitPlaneNormal(oct0, oct1, oct2, direction, n))
    {
      for (const ntg::triangle3& triangle : triangles)
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
  return triangles.size();
}

void OctreeNode::DestroyRecursive()
{
  if(!isLeaf_)
    for(OctreeNode *child : ToBranch()->children_)
      if(child) child->DestroyRecursive();
  delete this;
}

OctreeBranch* OctreeLeaf::Split()
{
  OctreeBranch* branch;
  if(parent_)
  {
    branch = new OctreeBranch(bounds_, parent_);
    OctreeNode *& parentChild = parent_->MatchChild(this);
    assert(parentChild == this);
    parentChild = branch;
  }
  else
  {
    branch = new OctreeBranch(bounds_, tree_);
    tree_->head_ = branch;
  }
  for (ntg::triangle3 tri : triangles_)
    branch->Add(tri, true);

  delete this;
  return branch;
}

bool OctreeLeaf::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  if (!ntg::collide(ray, bounds_))
    return false;
  float t;
  if(Raycast(ray, hit.triangle, t))
  {
    hit.point = ray.origin + t * ray.direction;
    return true;
  }
  return false;
}

bool OctreeLeaf::Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const
{
  if (!ntg::collide(ray, bounds_))
    return false;
  t_out = std::numeric_limits<float>::max();
  for (const ntg::triangle3& triangle : triangles_)
  {
    float t;
    if (collide(ray, triangle, t))
    {
      if(t < t_out)
      {
        t_out = t;
        triangle_out = triangle;
      }
    }
  }
  return t_out != std::numeric_limits<float>::max();
}


OctreeNode*& OctreeBranch::MatchChild(OctreeNode* node)
{
  for (OctreeNode*& child : children_)
    if(child == node)
      return child;
  throw std::out_of_range("Node pointer not found");
}


bool OctreeBranch::HasChild(OctreeNode* node)
{
  for (OctreeNode* child : children_)
    if (child == node)
      return true;
  return false;
}

OctreeNode*& OctreeBranch::MatchChild(const glm::vec3& point)
{
  return children_[octant(point)];
}

OctreeNode* OctreeBranch::MatchChild(const glm::vec3& point) const
{
  return children_[octant(point)];
}

OctreeLeaf* OctreeBranch::MatchLeaf(const glm::vec3& point)
{
  const signed oct = octant(point);
  OctreeNode *&child = children_[oct];

  if(!child)
    return (child = new OctreeLeaf(this, oct))->ToLeaf();
  if(child->isLeaf_)
    return child->ToLeaf();
  return child->ToBranch()->MatchLeaf(point);
}

unsigned char OctreeNode::octant(const glm::vec3& point) const
{
  const glm::vec3 v = point - bounds_.center();
  return ((v[0] >= 0)) |
    ((v[1] >= 0) << 1) |
    ((v[2] >= 0) << 2);
}

bool OctreeBranch::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  if (!ntg::collide(ray, bounds_))
    return false;
  float t;
  if (Raycast(ray, hit.triangle, t))
  {
    hit.point = ray.origin + t * ray.direction;
    return true;
  }
  return false;
}

bool OctreeBranch::Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const
{
  if(!ntg::collide(ray, bounds_))
    return false;

  t_out = std::numeric_limits<float>::max();
  for (const OctreeNode* const child : children_)
  {
    if (!child)
      continue;
    if (child->isLeaf_)
    {
      float t;
      ntg::triangle3 triangle;
      if(child->ToLeaf()->Raycast(ray, triangle, t))
      {
        if(t < t_out)
        {
          t_out = t;
          triangle_out = triangle;
        }
      }
    }
    else
    {
      float t;
      ntg::triangle3 triangle;
      if (child->ToBranch()->Raycast(ray, triangle, t))
      {
        if (t < t_out)
        {
          t_out = t;
          triangle_out = triangle;
        }
      }
    }
  }
  return t_out != std::numeric_limits<float>::max();
}

void OctreeBranch::Add(const ntg::triangle3& tri, bool noSplit)
{
  std::vector<ntg::triangle3> frontSplit;
  frontSplit.reserve(4);
  std::vector<ntg::triangle3> backSplit;
  frontSplit.reserve(4);

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
      ForceAdd(triangle, noSplit);
}

void OctreeBranch::ForceAdd(const ntg::triangle3& tri, bool noSplit)
{
  const unsigned char oct = octant(tri.toWorld({1.f/3.f, 1.f / 3.f, 1.f / 3.f}));
  if (children_[oct] == nullptr)
  {
    OctreeLeaf* l = new OctreeLeaf(this, oct);
    children_[oct] = l;
    l->Add(tri, noSplit);
    return;
  }
  if (children_[oct]->isLeaf_)
  {
    children_[oct]->ToLeaf()->Add(tri, noSplit);
    return;
  }
  children_[oct]->ToBranch()->Add(tri, noSplit);
}

OctreeBranch::OctreeBranch(const ntg::bounds3& bounds, Octree* tree) :
  OctreeNode(false, bounds, tree)
{
  children_.fill(nullptr);
}

OctreeBranch::OctreeBranch(const ntg::bounds3& bounds, OctreeBranch* parent):
  OctreeNode(false, bounds, parent)
{
  children_.fill(nullptr);
}

OctreeBranch::OctreeBranch(OctreeBranch* parent, unsigned char octant) :
  OctreeNode(false, parent, octant)
{
  children_.fill(nullptr);
}

Octree::Octree(const ntg::bounds3& bounds) :
head_(new OctreeLeaf(bounds, this))
{
}

Octree::~Octree()
{
  if(head_) head_->DestroyRecursive();
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

OctreeLeaf* Octree::MatchLeaf(const glm::vec3& point) const
{
  if (head_->isLeaf_)
    return head_->ToLeaf();
  return head_->ToBranch()->MatchLeaf(point);
}

bool Octree::Collide(const glm::vec3& point, ntg::triangle3& hit) const
{
  return false;
}

bool Octree::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  if (head_->isLeaf_)
    return head_->ToLeaf()->Raycast(ray, hit);
  return head_->ToBranch()->Raycast(ray, hit);
}

static bool ImguiHoverWhite()
{
  if (ImGui::IsItemHovered())
  {
    DebugDraw::SetColor(glm::vec4{ 1.f, 1.f, 1.f, 1.f });
    return true;
  }
  return false;
}
static std::vector<bool> headerHovered;

void Octree::ImguiDraw(GameObject* gameobject, const ntg::ray3& mouseRay)
{
  srand(0);
  ImGui::PushID(gameobject->ID());
  ImGui::Indent(10);
  const std::string name = gameobject->Name() + " Octree";
  const glm::mat4x4 t = gameobject->GetParentedComponent<TransformComponent>()->GetModelToWorld();
  VertexGlobalSystem::SetModelToWorld(t);
  if (ImGui::CollapsingHeader(name.c_str()))
  {
    if (head_->isLeaf_)
      head_->ToLeaf()->ImguiDraw(t, inverse(t) * mouseRay);
    else
      head_->ToBranch()->ImguiDraw(t, inverse(t) * mouseRay);
  }
  else if(ImguiHoverWhite())
  {
    if (head_->isLeaf_)
      head_->ToLeaf()->Render();
    else
      head_->ToBranch()->Render();
  }
  ImGui::Unindent(10);
  ImGui::PopID();
}

void OctreeBranch::ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay)
{
  ImGui::Indent(10);
  const ntg::bounds3 worldbounds = transform * bounds_;
  if(ntg::collide(mouseRay, bounds_))
    DebugDraw::SetColor(glm::vec4{ 1.f, 0.f, 1.f, 1.f });
  headerHovered.push_back(ImguiHoverWhite());
  DebugDraw::DrawAABB(bounds_, GL_LINE);

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
      if(std::find(headerHovered.begin(), headerHovered.end(), true) == headerHovered.end()) 
        DebugDraw::SetColor(glm::vec4{ color, 1.f });

      if(children_[i]->isLeaf_)
      {
        char buffer[32]{ 0 };
        sprintf_s(buffer, 32, "Leaf [%zu]", children_[i]->ToLeaf()->triangles_.size());
        if (ImGui::CollapsingHeader(buffer))
          children_[i]->ToLeaf()->ImguiDraw(transform, mouseRay);
        else
        {
          if (ntg::collide(mouseRay, children_[i]->bounds_))
            DebugDraw::SetColor(glm::vec4{ 1.f, 0.f, 1.f, 1.f });
          ImguiHoverWhite();

          DebugDraw::DrawAABB(children_[i]->bounds_, GL_LINE);
          children_[i]->ToLeaf()->Render(); 
        }
      }
      else
      {
        if (ImGui::CollapsingHeader("Branch"))
          children_[i]->ToBranch()->ImguiDraw(transform, mouseRay);
        else
        {
          if (ntg::collide(mouseRay, children_[i]->bounds_))
            DebugDraw::SetColor(glm::vec4{ 1.f, 0.f, 1.f,1.f });
          ImguiHoverWhite();

          DebugDraw::DrawAABB(children_[i]->bounds_, GL_LINE);
          children_[i]->ToBranch()->Render();
        }
      }
      ImGui::PopStyleColor();

      ImGui::PopID();
    }
  }
  ImGui::Unindent(10);
  headerHovered.pop_back();
}

void OctreeLeaf::ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay)
{
  ImGui::Indent(10);

  const ntg::bounds3 worldbounds = transform * bounds_;
    if (ntg::collide(mouseRay, bounds_))
      DebugDraw::SetColor(glm::vec4{ 1.f, 0.f, 1.f, 1.f });
  ImguiHoverWhite();
  DebugDraw::DrawAABB(bounds_, GL_LINE);

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
    ImGui::Unindent(10);
    ImGui::PopStyleColor();
    return;
  }
  char buffer[32]{0};
  sprintf_s(buffer, 32, "Triangles [%zu]", triangles_.size());

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

void OctreeLeaf::Render() const
{
  DebugDraw::DrawTriangleList(triangles_, GL_LINE);
}

void OctreeBranch::Render() const
{
  for (OctreeNode *child : children_)
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
