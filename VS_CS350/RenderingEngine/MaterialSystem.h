#pragma once
#include <bitset>
#include <GL/glew.h>

#include "LightSystem.h"
#include "Material.h"
#include "Utilities.h"

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
//    union
//    {
//      __declspec(align(16)) struct
//      {
//        AlignData<16, GLuint> curMaterial_;
//      } solidRender = {0}; // use when rendering solid objects
//
//      __declspec(align(16)) struct
//      {
//        AlignData<16, GLuint> curLight_;
//      } lightRender; // use when rendering bounded lights
//    };

    AlignData<16, GLuint> curMaterial_;
    AlignData<16, Material> materials_[materialCount];
    static constexpr GLuint binding_ = 1;
  } shaderData_;

  GLuint ssbo_ = 0;
  std::bitset<materialCount> dirtyMats_;
};

