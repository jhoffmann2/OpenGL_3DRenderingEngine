#include "ObjectComponent.h"
#include "GameObject.h"

ObjectComponent::~ObjectComponent() = default;

void ObjectComponent::Init()
{
}

void ObjectComponent::PreRender()
{
}

void ObjectComponent::Render()
{
}

void ObjectComponent::DebugRender()
{
}

void ObjectComponent::Kill()
{
}

void ObjectComponent::ImGuiEditor()
{
}

void ObjectComponent::OnObjectDeactivated()
{
}

void ObjectComponent::OnObjectActivated()
{
}

void ObjectComponent::SetObject(GameObject* obj)
{
  object_ = obj;
}

GameObject& ObjectComponent::Parent() const
{
  return *object_;
}
