#include "ChildListComponent.h"
#include "GameObject.h"
#include <fstream>


#include "FaceNormalRender.h"
#include "MaterialComponent.h"
#include "Mesh.h"
#include "RenderingComponent.h"
#include "SolidRender.h"
#include "TransformComponent.h"
#include "VertexNormalRender.h"

ChildListComponent::ChildListComponent() = default;

ChildListComponent::ChildListComponent(const std::vector<Mesh>& meshes, const std::vector<std::string>& names)
{
  for (size_t i = 0; i < meshes.size(); ++i)
  {
    const Mesh& mesh = meshes[i];
    const std::string& name = names[i];
    std::cout << "generating child: " << name << std::endl;

    const int mesh_id = SolidRender::loadMesh(mesh, SolidRender::DEFFERED);
    VertexNormalRender::loadMesh(mesh, 0.01f);
    FaceNormalRender::loadMesh(mesh, 0.01f);
    children_.emplace_back(new GameObject(name));
    auto* rendering = new RenderingComponent(mesh_id);
    children_.back()->AddComponent(rendering);
  }
  std::cout << "finished importing children meshes" << std::endl;
}

ChildListComponent::ChildListComponent(const std::vector<GameObject*> & children) :
  children_(children)
{
}

void ChildListComponent::AddChild(GameObject* object)
{
  children_.emplace_back(object);
}

const std::vector<GameObject*>& ChildListComponent::Children() const
{
  return children_;
}

void ChildListComponent::Render()
{
  for (GameObject* child : children_)
  {
    child->PreRender();
    child->Render();
  }
}

void ChildListComponent::Init()
{
  for (GameObject* child : children_)
  {
    child->Init();
  }
}

void ChildListComponent::DebugRender()
{
  for (GameObject* child : children_)
  {
    child->PreRender();
    child->DebugRender();
  }
}

void ChildListComponent::Kill()
{
  for (GameObject* child : children_)
  {
    child->Kill();
  }
}

void ChildListComponent::OnObjectDeactivated()
{
  for (GameObject* child : children_)
  {
    child->OnDeactivated();
  }
}

void ChildListComponent::OnObjectActivated()
{
  for (GameObject* child : children_)
  {
    child->OnActivated();
  }
}

void ChildListComponent::ImGuiEditor()
{
  for (GameObject* child : children_)
  {
    child->ImGuiEditor();
  }
}

std::string ChildListComponent::Name()
{
  return "Children";
}
