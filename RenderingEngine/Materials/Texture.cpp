#include "Texture.h"

#include <iostream>
#include <filesystem>

#include "SOIL/SOIL.h"
#include "rgbe.h"

Texture::Texture()
{
    if (file.extension() != ".hdr")
        LoadTextureFile(file);
    else
        LoadHDRTextureFile(file);
}

Texture::Texture(const std::string &filename)
    :file(std::filesystem::absolute(filename))
{
    if (file.extension() != ".hdr")
        LoadTextureFile(file);
    else
        LoadHDRTextureFile(file);
}

void Texture::LoadTextureFile(const std::filesystem::path &file)
{
    if (auto found = loadedTextures.find(file.string());
        found != loadedTextures.end())
    {
        textureID_ = found->second;
        return;
    }

    int width, height, channels;
    unsigned char *image = SOIL_load_image(
        file.string().c_str(),
        &width,
        &height,
        &channels,
        3);

    glGenTextures(1, &textureID_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
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
    loadedTextures[file.string()] = textureID_;
}

void Texture::LoadHDRTextureFile(const std::filesystem::path &file)
{
    if (auto found = loadedTextures.find(file.string());
        found != loadedTextures.end())
    {
        textureID_ = found->second;
        return;
    }

    int width, height;
    std::vector<float> image;
    rgbe_header_info info;
    char errbuf[100] = {0};

    // Open file and read width and height from the header
    FILE *fp = fopen(file.string().c_str(), "rb");
    if (!fp)
    {
        printf("Can't open file: %S\n", file.c_str());
        exit(-1);
    }
    int rc = RGBE_ReadHeader(fp, &width, &height, &info, errbuf);
    if (rc != RGBE_RETURN_SUCCESS)
    {
        printf("RGBE read error: %s\n", errbuf);
        exit(-1);
    }

    // Allocate enough memory
    image.resize(3 * width * height);

    // Read the pixel data and close the file
    rc = RGBE_ReadPixels_RLE(fp, &image[0], width, height, errbuf);
    if (rc != RGBE_RETURN_SUCCESS)
    {
        printf("RGBE read error: %s\n", errbuf);
        exit(-1);
    }
    fclose(fp);

    glGenTextures(1, &textureID_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB32F,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        image.data()
    );
    loadedTextures[file.string()] = textureID_;
}

GLuint Texture::TextureID()
{
    return textureID_;
}
