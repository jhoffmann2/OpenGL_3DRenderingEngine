#include "Texture.h"

#include <iostream>

#include "SOIL/SOIL.h"

Texture::Texture() = default;

Texture::Texture(const std::string& filename)
{
  LoadTextureFile(filename);
}

void Texture::LoadTextureFile(const std::string& filename)
{
  
  int width, height, channels;
  unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, &channels, 3);
  

  glGenTextures(1, &textureID_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D
  (
    GL_TEXTURE_2D, 
    0, 
    GL_RGB, 
    width, 
    height, 
    0, 
    GL_RGB, 
    GL_UNSIGNED_BYTE,
    image
  );
}

GLuint Texture::TextureID()
{
  if(textureID_ == NULL_TEXTURE)
    LoadTextureFile("../../Common/textures/white.png");
  return textureID_;
}
