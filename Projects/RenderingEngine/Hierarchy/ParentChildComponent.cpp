#include "ParentChildComponent.h"
#include "GameObjects/GameObject.h"
#include <fstream>
#include <utility>

#include "Materials/MaterialComponent.h"
#include "Mesh/Mesh.h"
#include "Rendering/FaceNormalRender.h"
#include "Rendering/RenderingComponent.h"
#include "Rendering/SolidRender.h"
#include "Rendering/VertexNormalRender.h"

ParentChildComponent::ParentChildComponent() = default;

ParentChildComponent::ParentChildComponent(
  std::unordered_set<ParentChildComponent*> children, 
  ParentChildComponent* parent
) : children_(std::move(children)), parent_(parent)
{
}

ParentChildComponent::ParentChildComponent(GameObject* parent) :
  parent_(parent->GetComponent<ParentChildComponent>())
{
  if(!parent_)
  {
    parent_ = new ParentChildComponent(this);
    parent->AddComponent(parent_);
  }
}

ParentChildComponent::ParentChildComponent(ParentChildComponent* parent) :
  parent_(parent)
{}

void ParentChildComponent::AddChild(ParentChildComponent* new_child)
{
  children_.emplace(new_child);
  new_child->parent_ = this;
}

void ParentChildComponent::AddChildrenFromMeshes(const std::vector<Mesh>& meshes, const std::vector<std::string>& names)
{
  for (size_t i = 0; i < meshes.size(); ++i)
  {
    const Mesh& mesh = meshes[i];
    const std::string& name = names[i];
    std::cout << "generating child: " << name << std::endl;

    const int mesh_id = SolidRender::loadMesh(mesh);
    VertexNormalRender::loadMesh(mesh, 0.01f);
    FaceNormalRender::loadMesh(mesh, 0.01f);

    auto obj = new GameObject(name);

    auto* parentChild = new ParentChildComponent(this);
    children_.emplace(parentChild);
    obj->AddComponent(parentChild);

    auto* rendering = new RenderingComponent(mesh_id, SolidRender::DEFFERED);
    rendering->flags_.set(RenderingComponent::RENDER_FLAG_DISABLE_FACE_NORMALS);
    obj->AddComponent(rendering);
  }
  std::cout << "finished importing children meshes" << std::endl;
}

const std::unordered_set<ParentChildComponent*>& ParentChildComponent::Children() const
{
  return children_;
}

void ParentChildComponent::SetParent(ParentChildComponent* new_parent)
{
  parent_ = new_parent;
  parent_->children_.emplace(this);
}

ParentChildComponent*ParentChildComponent::GetParent() const
{
  return parent_;
}

void ParentChildComponent::Render()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->PreRender();
    child->GetGameObject()->Render();
  }
}

void ParentChildComponent::Init()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->Init();
  }
}

void ParentChildComponent::ForwardRender()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->PreRender();
    child->GetGameObject()->ForwardRender();
  }
}

void ParentChildComponent::Kill()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->Kill();
  }
}

void ParentChildComponent::OnObjectDeactivated()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->OnDeactivated();
  }
}

void ParentChildComponent::OnObjectActivated()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->OnActivated();
  }
}

void ParentChildComponent::ImGuiEditor()
{
  for (ParentChildComponent* child : children_)
  {
    child->GetGameObject()->ImGuiEditor();
  }
}

std::string ParentChildComponent::Name()
{
  return "Children";
}
