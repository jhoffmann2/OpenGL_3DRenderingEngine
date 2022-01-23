#pragma once
#include <string>
#include "GL/glew.h"
#include <limits>

class Texture
{
public:
  Texture();
  Texture(const std::string& filename);
  void LoadTextureFile(const std::string &filename);
  GLuint TextureID();
private:
  constexpr static GLuint NULL_TEXTURE = std::numeric_limits<GLuint>::max();
  GLuint textureID_ = NULL_TEXTURE;
};

