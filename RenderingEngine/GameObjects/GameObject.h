#pragma once
#include <map>
#include <string>
#include <typeindex>
#include <utility>

#include "Hierarchy/ParentChildComponent.h"
#include "ObjectComponent.h"

class GameObject
{
public:
  virtual ~GameObject() = default;

  explicit GameObject(std::string name)
    : name_(std::move(name))
  {
  }

  virtual void Init();
  virtual void PreRender();
  virtual void Render();
  virtual void ForwardRender();
  virtual void Kill();
  virtual void ImGuiEditor();
  virtual void OnActivated();
  virtual void OnDeactivated();

  virtual void Activate();
  virtual void Deactivate();
  virtual void SetActive(bool active);
  virtual bool IsActive();

  template<typename ComponentType>
  [[nodiscard]] ComponentType *GetComponent() const
  {
    auto itr = components_.find(ComponentKey<ComponentType>());
    if(itr != components_.end())
      return reinterpret_cast<ComponentType *>(itr->second);
    return nullptr;
  }

  template<typename ComponentType>
  [[nodiscard]] ComponentType *GetParentedComponent() const
  {
    auto itr = components_.find(ComponentKey<ComponentType>());
    if(itr != components_.end())
      return reinterpret_cast<ComponentType *>(itr->second);

    auto* const pcgParent = GetComponent<ParentChildComponent>()->GetParent();
    if(pcgParent)
      return pcgParent->GetGameObject()->GetParentedComponent<ComponentType>();
    return nullptr;
  }

  template<typename ComponentType>
  [[nodiscard]] bool HasComponent() const
  {
    return components_.find(ComponentKey<ComponentType>()) != components_.end();
  }

  template<typename ComponentType>
  void AddComponent(const ComponentType *component)
  {
    ComponentType* nonconst = const_cast<ComponentType*>(component);
    nonconst->SetGameObject(this);
    components_[ComponentKey<ComponentType>()] = nonconst;
  }

  template<typename ComponentType>
  void RemoveComponent()
  {
    auto itr = components_.find(ComponentKey<ComponentType>());
    delete(itr->second);
    components_.erase(itr);
  }

  [[nodiscard]] const std::string &Name() const
  {
    return name_;
  }

  [[nodiscard]] int ID() const
  {
    return id_;
  }

private:
  template<typename ComponentType>
  [[nodiscard]] static std::type_index ComponentKey()
  {
    return typeid(ComponentType);
  }
  
  std::map<std::type_index, ObjectComponent *> components_;
  int id_ = nextID();
  std::string name_;
  bool active_ = true;
  static int nextID()
  {
    static int uniqueID = 0;
    return uniqueID++;
  }
};

