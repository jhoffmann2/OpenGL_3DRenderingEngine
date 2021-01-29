#pragma once
#include <string>


class GameObject;

class ObjectComponent
{
public:
  virtual ~ObjectComponent();
  virtual void Init();
  virtual void PreRender();
  virtual void Render();
  virtual void DebugRender();
  virtual void Kill();
  virtual void ImGuiEditor();
  virtual std::string Name() = 0;

  virtual void OnObjectDeactivated();
  virtual void OnObjectActivated();

  void SetObject(GameObject* obj);
  [[nodiscard]] GameObject& Parent() const;
private:
  GameObject* object_ = nullptr;
};

