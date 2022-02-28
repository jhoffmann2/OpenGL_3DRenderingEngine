#pragma once

#include <bitset>

#include "GameObjects/ObjectComponent.h"
#include "Materials/Material.h"
#include "Materials/Texture.h"
#include "SolidRender.h"
#include <sstream>
#include <filesystem>

class RenderingComponent : public ObjectComponent
{
public:
    using SHADER = SolidRender::SHADER;

    ~RenderingComponent() override;

    RenderingComponent(int meshIndex, SHADER shader = SHADER::DEFFERED);

    void Render() override;

    void PreRender() override;

    void ForwardRender() override;

    void SetTextures(const Texture &diffuse, const Texture &specular);

    void SetDiffuseTexture(const Texture &t);

    void SetSpecularTexture(const Texture &t);

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
        RENDER_FLAG_RENDER_INSIDE_OUT
    };
    std::bitset<32> flags_;

    bool solidRander_ = true;
    bool vnormRender_ = false;
    bool fnormRender_ = false;
private:
    void TextureCombo(std::filesystem::path path, Texture *texture);

    Texture diffuseTexture_;
    Texture specularTexture_;
    int meshIndex_ = -1;
    SHADER shader_;
};

