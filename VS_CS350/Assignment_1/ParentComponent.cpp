#include "ParentComponent.h"
#include "GameObject.h"
#include <fstream>


#include "FaceNormalRender.h"
#include "MaterialComponent.h"
#include "Mesh.h"
#include "RenderingComponent.h"
#include "SolidRender.h"
#include "TransformComponent.h"
#include "VertexNormalRender.h"

ParentComponent::ParentComponent() = default;

ParentComponent::ParentComponent(const std::vector<Mesh>& meshes, const std::vector<std::string>& names)
{
  for (size_t i = 0; i < meshes.size(); ++i)
  {
    const Mesh& mesh = meshes[i];
    const std::string& name = names[i];
    std::cout << "generating child: " << name << std::endl;

    const int mesh_id = SolidRender::loadMesh(mesh, SolidRender::DEFFERED);
    VertexNormalRender::loadMesh(mesh);
    FaceNormalRender::loadMesh(mesh);
    children_.emplace_back(new GameObject(name));
    auto* rendering = new RenderingComponent(mesh_id);
    children_.back()->AddComponent(rendering);
  }
  std::cout << "finished importing children meshes" << std::endl;
}

ParentComponent::ParentComponent(const std::vector<GameObject*> & children) :
  children_(children)
{
}

void ParentComponent::AddChild(GameObject* object)
{
  children_.emplace_back(object);
}

const std::vector<GameObject*>& ParentComponent::Children() const
{
  return children_;
}

void ParentComponent::PreRender()
{
  for (GameObject *child : children_)
  {
    child->PreRender();
  }
}

void ParentComponent::Render()
{
  for (GameObject* child : children_)
  {
    child->Render();
  }
}

void ParentComponent::Init()
{
  for (GameObject* child : children_)
  {
    child->Init();
  }
}

void ParentComponent::DebugRender()
{
  for (GameObject* child : children_)
  {
    child->DebugRender();
  }
}

void ParentComponent::Kill()
{
  for (GameObject* child : children_)
  {
    child->Kill();
  }
}

void ParentComponent::OnObjectDeactivated()
{
  for (GameObject* child : children_)
  {
    child->OnDeactivated();
  }
}

void ParentComponent::OnObjectActivated()
{
  for (GameObject* child : children_)
  {
    child->OnActivated();
  }
}

void ParentComponent::ImGuiEditor()
{
  for (GameObject* child : children_)
  {
    child->ImGuiEditor();
  }
}

std::string ParentComponent::Name()
{
  return "Children";
}
