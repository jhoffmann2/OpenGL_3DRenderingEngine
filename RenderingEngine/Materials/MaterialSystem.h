#pragma once
#include <bitset>
#include <GL/glew.h>

#include "Lighting/LightSystem.h"
#include "Utilities/Utilities.h"
#include "Material.h"

class MaterialSystem
{
public:
  static constexpr size_t materialCount = LightSystem::lightCount + 20;
  friend class MaterialHandle;

  static void Init();
  static void Update();
  static void SetDirty(const Material& mat);
  static void SetDirty(size_t index);

  static void SetCurrentMaterialIndex(size_t index);

  [[nodiscard]] static const Material& GetCurrentMaterial();
  [[nodiscard]] static size_t GetCurrentMaterialIndex();

private:
  static MaterialSystem& Instance();
  MaterialSystem() = default;
  static Material& GetRawMaterialData(size_t index);
  static size_t MaterialIndex(const Material& material);

  // force 16 byte alignment between arrays
  __declspec(align(16)) struct ShaderData
  {
    AlignData<16, GLuint> curMaterial_;
    AlignData<16, Material> materials_[materialCount];
    static constexpr GLuint binding_ = 1;
  } shaderData_;

  GLuint ssbo_ = 0;
  std::bitset<materialCount> dirtyMats_;
};

