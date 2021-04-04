#include "BspTree.h"
#include <array>
#include <glm/gtc/random.inl>
#include <glm/gtx/color_space.inl>



#include "DebugDraw.h"
#include "GameObject.h"
#include "imgui.h"
#include "VertexGlobalSystem.h"
#include "Mesh.h"
#include "ntg/hyperplane.inl"
#include "ntg/collision.inl"

struct BspNode
{
  BspNode(bool isLeaf, BspTree* tree);
  BspNode(bool isLeaf, BspBranch* parent);
  [[nodiscard]] BspLeaf* ToLeaf();
  [[nodiscard]] BspBranch* ToBranch();
  [[nodiscard]] const BspLeaf* ToLeaf() const;
  [[nodiscard]] const BspBranch* ToBranch() const;
  void DestroyRecursive();
  bool isLeaf_;
  BspBranch* parent_ = nullptr;
  BspTree* tree_ = nullptr;
};

struct BspLeaf : BspNode
{
  BspLeaf(std::vector<ntg::triangle3>&& triangles, BspTree* tree);
  BspLeaf(std::vector<ntg::triangle3>&& triangles, BspBranch* parent);
  BspBranch* Split();
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const;
  void ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay);
  void Render() const;
  std::vector<ntg::triangle3> triangles_;
  size_t splitSize_ = 0;
};

struct BspBranch : BspNode
{
  BspBranch(std::vector<ntg::triangle3>&& triangles, ntg::hyperplane3 splitPlane, BspTree* tree);
  BspBranch(std::vector<ntg::triangle3>&& triangles, ntg::hyperplane3 splitPlane, BspBranch* parent);
  static bool SplitPlaneHeuristic(const std::vector<ntg::triangle3>& triangles, 
                            ntg::hyperplane3& plane_out);
  [[nodiscard]] BspNode*& MatchChild(BspNode* node);
  [[nodiscard]] bool HasChild(BspNode* node);
  [[nodiscard]] BspNode*& MatchChild(const glm::vec3& point);
  [[nodiscard]] BspNode* MatchChild(const glm::vec3& point) const;
  [[nodiscard]] BspLeaf* MatchLeaf(const glm::vec3& point);
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::hit3& hit) const;
  [[nodiscard]] bool Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const;
  void ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay);
  void Render() const;

  ntg::hyperplane3 plane_;
  std::array<BspNode*, 2> children_;
};

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


BspTree::BspTree(const std::vector<Mesh>& meshes)
{
  std::vector<ntg::triangle3> triangles;
  for (const Mesh& mesh : meshes)
  {
    for (const glm::uvec3& face : mesh.faces)
    {
      triangles.emplace_back(ntg::triangle3{
        mesh.vertices[face[0]],
        mesh.vertices[face[1]],
        mesh.vertices[face[2]]
      });

      assert(glm::distance(mesh.vertices[face[0]], mesh.vertices[face[1]]) > 1e-6);
      assert(glm::distance(mesh.vertices[face[0]], mesh.vertices[face[2]]) > 1e-6);
      assert(glm::distance(mesh.vertices[face[1]], mesh.vertices[face[2]]) > 1e-6);
    }
  }

  ntg::hyperplane3 splitPlane;
  if(triangles.size() >= 500 && BspBranch::SplitPlaneHeuristic(triangles, splitPlane))
    head_ = new BspBranch(std::move(triangles), splitPlane, this);
  else
    head_ = new BspLeaf(std::move(triangles), this);
}

BspTree::~BspTree()
{
  if (head_) head_->DestroyRecursive();
}

bool BspTree::Collide(const glm::vec3& point, ntg::triangle3& hit) const
{
  return false;
}

bool BspTree::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  if (head_->isLeaf_)
    return head_->ToLeaf()->Raycast(ray, hit);
  return head_->ToBranch()->Raycast(ray, hit);
}

void BspTree::ImguiDraw(GameObject* gameobject, const ntg::ray3& mouseRay)
{
  srand(0);
  ImGui::PushID(gameobject->ID());
  ImGui::Indent(10);
  const std::string name = gameobject->Name() + " BspTree";
  if (ImGui::CollapsingHeader(name.c_str()))
  {
    const glm::mat4x4 t = gameobject->GetParentedComponent<TransformComponent>()->GetModelToWorld();
    VertexGlobalSystem::SetModelToWorld(t);

    if (head_->isLeaf_)
      head_->ToLeaf()->ImguiDraw(t, inverse(t) * mouseRay);
    else
      head_->ToBranch()->ImguiDraw(t, inverse(t) * mouseRay);
  }
  ImGui::Unindent(10);
  ImGui::PopID();
}

BspNode::BspNode(bool isLeaf, BspTree* tree) :
  isLeaf_(isLeaf), parent_(nullptr), tree_(tree)
{
}

BspNode::BspNode(bool isLeaf, BspBranch* parent) :
  isLeaf_(isLeaf), parent_(parent), tree_(parent->tree_)
{
}

BspLeaf* BspNode::ToLeaf()
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<BspLeaf*>(this);
}

BspBranch* BspNode::ToBranch()
{
  if (isLeaf_)
    return nullptr;
  return static_cast<BspBranch*>(this);
}

const BspLeaf* BspNode::ToLeaf() const
{
  if (!isLeaf_)
    return nullptr;
  return static_cast<const BspLeaf*>(this);
}

const BspBranch* BspNode::ToBranch() const
{
  if (isLeaf_)
    return nullptr;
  return static_cast<const BspBranch*>(this);
}

void BspNode::DestroyRecursive()
{
  if (!isLeaf_)
    for (BspNode* child : ToBranch()->children_)
      if(child) child->DestroyRecursive();
  delete this;
}

BspLeaf::BspLeaf(std::vector<ntg::triangle3>&& triangles, BspTree* tree) :
  BspNode(true, tree), triangles_(std::move(triangles))
{}

BspLeaf::BspLeaf(std::vector<ntg::triangle3>&& triangles, BspBranch* parent) :
  BspNode(true, parent), triangles_(std::move(triangles))
{}

BspBranch* BspLeaf::Split()
{
  BspBranch *branch;
  ntg::hyperplane3 splitPlane;
  BspBranch::SplitPlaneHeuristic(triangles_, splitPlane);
  if (parent_)
  {
    branch = new BspBranch(std::move(triangles_), splitPlane, parent_);
    BspNode*& parentChild = parent_->MatchChild(this);
    assert(parentChild == this);
    parentChild = branch;
  }
  else
  {
    branch = new BspBranch(std::move(triangles_), splitPlane, tree_);
    tree_->head_ = branch;
  }
  delete this;
  return branch;
}

bool BspLeaf::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  float t;
  if (Raycast(ray, hit.triangle, t))
  {
    hit.point = ray.origin + t * ray.direction;
    return true;
  }
  return false;
}

bool BspLeaf::Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const
{
  t_out = std::numeric_limits<float>::max();
  for (const ntg::triangle3& triangle : triangles_)
  {
    float t;
    if (collide(ray, triangle, t))
    {
      if (t < t_out)
      {
        t_out = t;
        triangle_out = triangle;
      }
    }
  }
  return t_out != std::numeric_limits<float>::max();
}

void BspLeaf::ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay)
{
  ImGui::Indent(10);

  ImguiHoverWhite();

  Render();

  ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 1.f });

  if (ImGui::Button("Split"))
  {
    Split();
    ImGui::Unindent(10);
    ImGui::PopStyleColor();
    return;
  }
  char buffer[32]{ 0 };
  sprintf_s(buffer, 32, "Triangles [%llu]", triangles_.size());

  if (ImGui::CollapsingHeader(buffer))
  {
    for (const ntg::triangle3& tri : triangles_)
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

void BspLeaf::Render() const
{
  DebugDraw::DrawTriangleList(triangles_, GL_LINE);
}

BspBranch::BspBranch(std::vector<ntg::triangle3>&& triangles, ntg::hyperplane3 splitPlane, BspTree* tree) :
  BspNode(false, tree), plane_(splitPlane)
{
  std::vector<ntg::triangle3> frontTriangles;
  std::vector<ntg::triangle3> backTriangles;
  ntg::split(triangles, plane_, frontTriangles, backTriangles);

  ntg::hyperplane3 childPlane;
  // if there's a bunch of triangles and a good split exists, split again. else just create a leaf
  if(frontTriangles.size() >= 500 && SplitPlaneHeuristic(frontTriangles, childPlane))
    children_.front() = new BspBranch(std::move(frontTriangles), childPlane, this);
  else
    children_.front() = frontTriangles.empty() ? nullptr : new BspLeaf(std::move(frontTriangles), this);

  // if there's a bunch of triangles and a good split exists, split again. else just create a leaf
  if (backTriangles.size() >= 500 && SplitPlaneHeuristic(backTriangles, childPlane))
    children_.back() = new BspBranch(std::move(backTriangles), childPlane, this);
  else
    children_.back() = backTriangles.empty() ? nullptr : new BspLeaf(std::move(backTriangles), this);
}

BspBranch::BspBranch(std::vector<ntg::triangle3>&& triangles, ntg::hyperplane3 splitPlane, BspBranch* parent) :
  BspNode(false, parent), plane_(splitPlane)
{
  std::vector<ntg::triangle3> frontTriangles;
  std::vector<ntg::triangle3> backTriangles;
  ntg::split(triangles, plane_, frontTriangles, backTriangles);

  ntg::hyperplane3 childPlane;
  // if there's a bunch of triangles and a good split exists, split again. else just create a leaf
  if (frontTriangles.size() >= 500 && SplitPlaneHeuristic(frontTriangles, childPlane))
    children_.front() = new BspBranch(std::move(frontTriangles), childPlane, this);
  else
    children_.front() = frontTriangles.empty() ? nullptr : new BspLeaf(std::move(frontTriangles), this);

  // if there's a bunch of triangles and a good split exists, split again. else just create a leaf
  if (backTriangles.size() >= 500 && SplitPlaneHeuristic(backTriangles, childPlane))
    children_.back() = new BspBranch(std::move(backTriangles), childPlane, this);
  else
    children_.back() = backTriangles.empty() ? nullptr : new BspLeaf(std::move(backTriangles), this);
}

bool BspBranch::SplitPlaneHeuristic(const std::vector<ntg::triangle3>& triangles, ntg::hyperplane3& plane_out)
{
  ntg::vec3 averageVertex(0);
  float weight = 1.f / (triangles.size() * 3.f);
  for (const ntg::triangle3& triangle : triangles)
    for (const glm::vec3& vertex : triangle.points)
      averageVertex += vertex * weight;

  constexpr float sq3v3 = 0.57735026919f;
  static const std::array<glm::vec3, 7> splitPlaneNormals{ {
    {1.f, 0.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 0.f, 1.f},
    {sq3v3, sq3v3, sq3v3},
    {-sq3v3, sq3v3, sq3v3},
    {sq3v3, -sq3v3, sq3v3},
    {sq3v3, sq3v3, -sq3v3},
  } };
  size_t bestSplitWeight = std::numeric_limits<size_t>::max();
  size_t bestSplitCount = 0;
  for (const glm::vec3& normal : splitPlaneNormals)
  {
    const ntg::hyperplane3 splitPlane{ averageVertex, normal };

    size_t frontCount = 0;
    size_t backCount = 0;
    ntg::splitCount(triangles, splitPlane, frontCount, backCount);
    const size_t splitWeight = glm::max(frontCount, backCount) - glm::min(frontCount, backCount);

    if (splitWeight < bestSplitWeight)
    {
      bestSplitWeight = splitWeight;
      plane_out = splitPlane;
      bestSplitCount = frontCount + backCount;
    }
  }
  return bestSplitCount < 1.5 * triangles.size();
}

BspNode*& BspBranch::MatchChild(BspNode* node)
{
  for (BspNode*& child : children_)
    if (child == node)
      return child;
  throw std::out_of_range("Node pointer not found");
}

bool BspBranch::HasChild(BspNode* node)
{
  for (BspNode* child : children_)
    if (child == node)
      return true;
  return false;
}

BspNode*& BspBranch::MatchChild(const glm::vec3& point)
{
  if(ntg::ternary_collide(plane_, point) >= 0)
    return children_.front();
  return children_.back();
}

BspNode* BspBranch::MatchChild(const glm::vec3& point) const
{
  if (ntg::ternary_collide(plane_, point) >= 0)
    return children_.front();
  return children_.back();
}

BspLeaf* BspBranch::MatchLeaf(const glm::vec3& point)
{
  BspNode *child = MatchChild(point);
  if(!child) return nullptr;
  if(child->isLeaf_)
    return child->ToLeaf();
  return child->ToBranch()->MatchLeaf(point);
}

bool BspBranch::Raycast(const ntg::ray3& ray, ntg::hit3& hit) const
{
  float t;
  if (Raycast(ray, hit.triangle, t))
  {
    hit.point = ray.origin + t * ray.direction;
    return true;
  }
  return false;
}

bool BspBranch::Raycast(const ntg::ray3& ray, ntg::triangle3& triangle_out, float& t_out) const
{
  const bool collide = ntg::collide(ray, plane_);
  const int ternaryCollide = ntg::ternary_collide(ray.origin, plane_);
  const bool checkLeaf[2] = {
    collide || ternaryCollide >= 0,
    collide || ternaryCollide < 0,
  };

  t_out = std::numeric_limits<float>::max();
  for (size_t i = 0; i < children_.size(); ++i)
  {
    const BspNode* const child = children_[i];
    if (!checkLeaf[i]) continue;
    if (!child) continue;

    if (child->isLeaf_)
    {
      float t;
      ntg::triangle3 triangle;
      if (child->ToLeaf()->Raycast(ray, triangle, t))
      {
        if (t < t_out)
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

void BspBranch::ImguiDraw(const glm::mat4& transform, const ntg::ray3& mouseRay)
{
  ImGui::Indent(10);
  const ntg::hyperplane3 worldPlane = transform * plane_;
  headerHovered.push_back(ImguiHoverWhite());

  ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 1.f });
  ImGui::Text("SplitPlane: O[%.2f, %.2f, %.2f], N[%.2f, %.2f, %.2f]",
    worldPlane.origin.x,
    worldPlane.origin.y,
    worldPlane.origin.z,
    worldPlane.normal.x,
    worldPlane.normal.y,
    worldPlane.normal.z
  );
  ImGui::PopStyleColor();

  for (size_t i = 0; i < children_.size(); ++i)
  {
    if (children_[i])
    {
      ImGui::PushID(static_cast<int>(i));

      const glm::vec3 color = rgbColor(
        linearRand(
          glm::vec3{ 0.f, 0.5f, 0.5f },
          glm::vec3{ 360.f, 1.f, 1.f }
        )
      );
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ color.r, color.g, color.b, 1.f });
      if (std::find(headerHovered.begin(), headerHovered.end(), true) == headerHovered.end())
        DebugDraw::SetColor(glm::vec4{ color, 1.f });

      if (children_[i]->isLeaf_)
      {
        char buffer[32]{ 0 };
        sprintf_s(buffer, 32, "Leaf [%llu]", children_[i]->ToLeaf()->triangles_.size());
        if (ImGui::CollapsingHeader(buffer))
          children_[i]->ToLeaf()->ImguiDraw(transform, mouseRay);
        else
        {
          ImguiHoverWhite();
          children_[i]->ToLeaf()->Render();
        }
      }
      else
      {
        if (ImGui::CollapsingHeader("Branch"))
          children_[i]->ToBranch()->ImguiDraw(transform, mouseRay);
        else
        {
          ImguiHoverWhite();
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

void BspBranch::Render() const
{
  for (BspNode* child : children_)
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
