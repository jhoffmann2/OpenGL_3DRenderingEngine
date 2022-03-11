#pragma once

#include <bitset>
#include <GL/glew.h>
#include <Materials/Texture.h>
#include "Light.h"
#include "Utilities/LowDiscrepancyRandom.h"

class LightSystem
{
public:
  static constexpr size_t lightCount = 75;
  friend class LightHandle;
  static void Init();
  static void Update();
  static void SetDirty(const Light& light);
  static void SetDirty(size_t index);

  static void SetLightAttenuation(const glm::vec3& attenuation);
  static void SetFog(const glm::vec3& color, float near, float far);
  static void SetAmbientColor(const glm::vec3& color);
  static void SetEyePos(const glm::vec3& pos);
  static void SetEnvironmentLightStrength(float strength);
  static void SetSpecularSamplingLevel(int level);

  [[nodiscard]] static const glm::vec3& GetLightAttenuation();
  [[nodiscard]] static const glm::vec3& GetFogColor();
  [[nodiscard]] static std::pair<float,float> GetFogRange();
  [[nodiscard]] static const glm::vec3& GetAmbientColor();
  [[nodiscard]] static const glm::vec3& GetEyePos();
  [[nodiscard]] static float GetEnvironmentLightStrength();
  [[nodiscard]] static int GetSpecularSamplingLevel();
  [[nodiscard]] static size_t MaxSpecularSamplingLevel();

private:
  static LightSystem& Instance();
  LightSystem() = default;
  static Light& GetRawLightData(size_t index);
  static size_t LightIndex(const Light& light);

  // force 16 byte alignment between arrays
  __declspec(align(16)) struct ShaderData
  {
    static constexpr GLuint binding_ = 0;
    struct GlobalProperties
    {
      AlignData<16, glm::vec3> lightAttenuation_;

      AlignData<12, glm::vec3> fogColor_;
      AlignData<4, float> fogNear_;
      AlignData<16, float> fogFar_;

      AlignData<16, glm::vec3> ambientColor_;
      AlignData<12, glm::vec3> eyePos_;

      AlignData<4, float> environmentLightStrength_ = 1.f;
      AlignData<16, GLuint> specularSamplingLevel = 40;
      std::array<glm::vec4, 100> hammersley = Hammersley2D<100>();
    } globalProperties_;

    AlignData<16, Light> lights_[lightCount];

  } shaderData_;

  GLuint ssbo_ = 0;
  std::bitset<lightCount> dirtyLights_;
  bool dirtyGlobalProps_ = false;
};

