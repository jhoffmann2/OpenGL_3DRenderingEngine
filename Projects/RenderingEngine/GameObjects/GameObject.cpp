#include "GameObject.h"
#include "imgui/imgui.h"

void GameObject::Init()
{
  for(auto& pair : components_)
  {
    ObjectComponent* component = pair.second;
    component->Init();
  }
}

void GameObject::PreRender()
{
  if(active_)
  {
    for (auto& pair : components_)
    {
      ObjectComponent* component = pair.second;
      component->PreRender();
    }
  }
}

void GameObject::Render()
{
  if (active_)
  {
    for (auto& pair : components_)
    {
      ObjectComponent* component = pair.second;
      component->Render();
    }
  }
}

void GameObject::ForwardRender()
{
  if (active_)
  {
    for (auto& pair : components_)
    {
      ObjectComponent* component = pair.second;
      component->ForwardRender();
    }
  }
}

void GameObject::Kill()
{
  for (auto& pair : components_)
  {
    ObjectComponent* component = pair.second;
    component->Kill();
    delete component;
  }
}

void GameObject::ImGuiEditor()
{
  if (active_)
  {
    ImGui::PushID(id_);
    if (ImGui::CollapsingHeader(name_.c_str()))
    {
      for (auto& pair : components_)
      {
        ObjectComponent* component = pair.second;
        ImGui::Indent(20);
        ImGui::PushItemWidth(400);

        if (ImGui::CollapsingHeader(component->Name().c_str()))
        {
          ImGui::Indent(20);
          component->ImGuiEditor();
          ImGui::Unindent(20);
        }
        ImGui::PopItemWidth();
        ImGui::Unindent(20);
        ImGui::Separator();
      }
    }
    ImGui::PopID();
  }
}

void GameObject::OnActivated()
{
  for (auto& pair : components_)
  {
    ObjectComponent* component = pair.second;
    component->OnObjectActivated();
  }
}

void GameObject::OnDeactivated()
{
  for (auto& pair : components_)
  {
    ObjectComponent* component = pair.second;
    component->OnObjectDeactivated();
  }
}

void GameObject::Activate()
{
  active_ = true;
  OnActivated();
}

void GameObject::Deactivate()
{
  active_ = false;
  OnDeactivated();
}

void GameObject::SetActive(bool active)
{
  active_ = active;
  active ? OnActivated() : OnDeactivated();
}

bool GameObject::IsActive()
{
  return active_;
}

