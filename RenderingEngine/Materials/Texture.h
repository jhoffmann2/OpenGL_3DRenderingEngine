#pragma once
#include <string>
#include "GL/glew.h"
#include <limits>
#include <unordered_map>
#include <filesystem>

class Texture
{
public:
  Texture();
  Texture(const std::string& filename);
  void LoadHDRTextureFile(const std::filesystem::path &file);
  void LoadTextureFile(const std::filesystem::path &file);
  GLuint TextureID() const;
  const std::filesystem::path& Path() const {return file;}
private:
  constexpr static GLuint NULL_TEXTURE = std::numeric_limits<GLuint>::max();
  static inline std::unordered_map<std::string, GLuint> loadedTextures = {};
  GLuint textureID_ = NULL_TEXTURE;
  std::filesystem::path file = {"../../Common/textures/white.png"};
};

