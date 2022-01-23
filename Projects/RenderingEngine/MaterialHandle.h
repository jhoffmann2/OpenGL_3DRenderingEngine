#pragma once
#include "Utilities.h"
#include "MaterialSystem.h"

class MaterialHandle
{
public:
  friend class MaterialSystem;
  static MaterialHandle null();
  using TextureMode = Material::TextureMode;

  MaterialHandle();
  MaterialHandle(size_t index);
  MaterialHandle& operator=(const MaterialHandle other);

  [[nodiscard]] const glm::vec3& GetEmissiveColor() const;
  [[nodiscard]] const glm::vec3& GetDiffuseColor() const;
  [[nodiscard]] const glm::vec3& GetSpecularColor() const;
  [[nodiscard]] const glm::vec3& GetAmbientColor() const;

  [[nodiscard]] float GetSpecularExponent() const;

  [[nodiscard]] TextureMode GetTextureMode() const;

  [[nodiscard]] size_t GetIndex() const;


  void SetEmissiveColor(const glm::vec3& color);
  void SetDiffuseColor(const glm::vec3& color);
  void SetSpecularColor(const glm::vec3& color);
  void SetAmbientColor(const glm::vec3& color);

  void SetSpecularExponent(float exp);

  void SetTextureMode(TextureMode mode);

  void SetIndex(size_t index);
  void Clone(const MaterialHandle other);

private:
  [[nodiscard]] Material& getData() const;

  size_t index_;
};

