#pragma once
#include <bitset>

#include "Material.h"
#include "ObjectComponent.h"
#include "SolidRender.h"
#include "Texture.h"

class RenderingComponent : public ObjectComponent
{
public:
  using SHADER = SolidRender::SHADER;
  ~RenderingComponent() override;
  RenderingComponent(int meshIndex, SHADER shader = SHADER::DEFFERED);
  void Render() override;
  void DebugRender() override;

  void SetTextures(const Texture& diffuse, const Texture& specular);
  void SetDiffuseTexture(const Texture& t);
  void SetSpecularTexture(const Texture& t);

  void ImGuiEditor() override;
  std::string Name() override;

  void SetMeshIndex(int mesh);
  [[nodiscard]] int GetMeshIndex() const;

  void SetShader(SHADER shader);
  [[nodiscard]] SHADER GetShader();


  enum Flags
  {
    RENDER_FLAG_DISABLE_FACE_NORMALS,
    RENDER_FLAG_DISABLE_VERTEX_NORMALS,
    RENDER_FLAG_DISABLE_SOLID_RENDERING,
  };
  std::bitset<32> flags_;

  bool solidRander_ = true;
  bool vnormRender_ = false;
  bool fnormRender_ = false;
private:
  Texture diffuseTexture_;
  Texture specularTexture_;
  int meshIndex_ = -1;
  SHADER shader_;
};

