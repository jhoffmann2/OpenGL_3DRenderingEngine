#include "RenderingComponent.h"
#include <filesystem>
#include <utility>

#include "GameObjects/GameObject.h"
#include "Materials/MaterialSystem.h"
#include "FaceNormalRender.h"
#include "SolidRender.h"
#include "VertexNormalRender.h"
#include "imgui.h"
#include <filesystem>
#include <sstream>

RenderingComponent::~RenderingComponent() = default;

RenderingComponent::RenderingComponent(int meshIndex, SHADER shader) :
    meshIndex_(meshIndex), shader_(shader)
{
}

void RenderingComponent::SetMeshIndex(int mesh)
{
    meshIndex_ = mesh;
}

int RenderingComponent::GetMeshIndex() const
{
    return meshIndex_;
}

void RenderingComponent::SetShader(SHADER shader)
{
    shader_ = shader;
}

RenderingComponent::SHADER RenderingComponent::GetShader()
{
    return shader_;
}


void RenderingComponent::PreRender()
{
    if (SolidRender::GetShader() != SolidRender::DEPTH_MAP)
    {
        SolidRender::SetShader(shader_);
    }
}

void RenderingComponent::Render()
{
    if(flags_[RENDER_FLAG_RENDER_INSIDE_OUT])
        glCullFace(GL_FRONT);

    if (solidRander_ && shader_ == SHADER::DEFFERED && !flags_[RENDER_FLAG_DISABLE_SOLID_RENDERING])
        SolidRender::draw(meshIndex_, diffuseTexture_, specularTexture_);

    glCullFace(GL_BACK);
}

void RenderingComponent::ForwardRender()
{
    if(flags_[RENDER_FLAG_RENDER_INSIDE_OUT])
        glCullFace(GL_FRONT);

    if (solidRander_
        && (shader_ != SHADER::DEFFERED || SolidRender::GetShader() == SolidRender::DEPTH_MAP)
        && shader_ != SHADER::LOCAL_LIGHT
        && !flags_[RENDER_FLAG_DISABLE_SOLID_RENDERING])
    {
        SolidRender::draw(meshIndex_, diffuseTexture_, specularTexture_);
    }

    if (solidRander_
        && shader_ == SHADER::LOCAL_LIGHT
        && !flags_[RENDER_FLAG_DISABLE_SOLID_RENDERING])
    {
        SolidRender::SetShader(shader_);
        SolidRender::drawDeferredSolid(meshIndex_);
    }

    if (vnormRender_ && !flags_[RENDER_FLAG_DISABLE_VERTEX_NORMALS]) VertexNormalRender::draw(meshIndex_);
    if (fnormRender_ && !flags_[RENDER_FLAG_DISABLE_FACE_NORMALS]) FaceNormalRender::draw(meshIndex_);

    glCullFace(GL_BACK);
}


void RenderingComponent::SetTextures(const Texture &diffuse, const Texture &specular)
{
    diffuseTexture_ = diffuse;
    specularTexture_ = specular;
}

void RenderingComponent::SetDiffuseTexture(const Texture &t)
{
    diffuseTexture_ = t;
}

void RenderingComponent::SetSpecularTexture(const Texture &t)
{
    specularTexture_ = t;
}

void RenderingComponent::ImGuiEditor()
{
    namespace fs = std::filesystem;
    std::vector<char *> objFiles;
    std::vector<fs::path> objFilePaths;
    static char modelDir[256] = "../../Common/models/";
    ImGui::Text("Mesh");
    ImGui::InputText("Directory", modelDir, IM_ARRAYSIZE(modelDir));

    for (const fs::directory_entry &entry: fs::directory_iterator(modelDir))
        if (entry.path().extension() == ".obj")
        {
            std::string nameStr = entry.path().filename().string();
            char *path = new char[nameStr.size() + 1];
            strcpy_s(path, nameStr.size() + 1, nameStr.data());
            objFiles.push_back(path);
            objFilePaths.push_back(entry.path());
        }
    static int currentModel;
    ImGui::PushItemWidth(300);
    ImGui::Combo("Model", &currentModel, objFiles.data(), static_cast<int>(objFiles.size()));
    ImGui::SameLine();
    static UV_MAPPING mapping = UV_NONE;
    ImGui::Combo(
        "CPU UV Mapping",
        reinterpret_cast<int *>(&mapping),
        "NONE\0"
        "PLANAR\0"
        "CYLINDRICAL\0"
        "SPHERICAL\0"
    );
    ImGui::PopItemWidth();

    if (ImGui::SmallButton("Load"))
    {
        const auto obj_meshData(ObjToMesh(objFilePaths[currentModel].string(), mapping));
        meshIndex_ = SolidRender::loadMesh(obj_meshData);
        VertexNormalRender::loadMesh(obj_meshData);
        FaceNormalRender::loadMesh(obj_meshData);
    }
    for (char *p: objFiles)
        delete[] p;

    ImGui::Separator();

    if (!flags_[RENDER_FLAG_DISABLE_VERTEX_NORMALS])
    {
        ImGui::Checkbox("Render Object", &solidRander_);
        if (solidRander_)
        {
            ImGui::Indent(20);
            ImGui::PushItemWidth(400);

            ImGui::Combo("Shader", reinterpret_cast<int *>(&shader_),
                "PhongLighting\0"
                "PhongShading\0"
                "BlinnLighting\0"
                "BlinnShading\0"
                "FlatEmission\0"
                "Deferred Phong\0"
                "Local Light\0"
                "Depth Map\0"
            );
            if (ImGui::Button("Reload Shaders"))
                SolidRender::ReloadShaders();

            ImGui::PopItemWidth();
            ImGui::Unindent(20);
        }
    }
    if (!flags_[RENDER_FLAG_DISABLE_VERTEX_NORMALS])
    {
        ImGui::Checkbox("Render Vertex Normals", &vnormRender_);
        if (vnormRender_)
        {
            ImGui::Indent(20);
            ImGui::PushItemWidth(400);

            static glm::vec4 color = {1, 0, 1, 1};
            ImGui::ColorEdit3("color", glm::data(color));
            VertexNormalRender::setRGB(color);

            ImGui::PopItemWidth();
            ImGui::Unindent(20);
        }
    }
    if (!flags_[RENDER_FLAG_DISABLE_FACE_NORMALS])
    {
        ImGui::Checkbox("Render Face Normals", &fnormRender_);
        if (fnormRender_)
        {
            ImGui::Indent(20);
            ImGui::PushItemWidth(400);

            static glm::vec4 color = {0, 1, 1, 1};
            ImGui::ColorEdit3("color", data(color));
            FaceNormalRender::setRGB(color);

            ImGui::PopItemWidth();
            ImGui::Unindent(20);
        }
    }

    if (ImGui::BeginCombo("DiffuseTexture", diffuseTexture_.Path().filename().string().c_str()))
    {
        TextureCombo("../../Common/Textures", &diffuseTexture_);
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("SpecularTexture", specularTexture_.Path().filename().string().c_str()))
    {
        TextureCombo("../../Common/Textures", &specularTexture_);
        ImGui::EndCombo();
    }
}

std::string RenderingComponent::Name()
{
    return "Rendering Component";
}

void RenderingComponent::TextureCombo(std::filesystem::path path, Texture *texture)
{
    path = std::filesystem::relative(path, std::filesystem::current_path());
    for(auto &item : std::filesystem::directory_iterator(path))
    {
        const auto& itemPath = item.path();
        if(item.is_directory())
        {
            TextureCombo(itemPath, texture);
        }
        else
        {
            // only support png, jpg, and hdr images
            if(itemPath.extension() != ".png" &&
                itemPath.extension() != ".jpg" &&
                itemPath.extension() != ".hdr")
            {
                continue;
            }

            const bool is_selected = (itemPath == texture->Path());
            if (ImGui::Selectable(itemPath.filename().string().c_str(), is_selected))
                *texture = itemPath.string();

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
    }
}
