#include "RenderingComponent.h"
#include <filesystem>
#include <utility>

#include "MaterialSystem.h"
#include "SolidRender.h"
#include "VertexNormalRender.h"
#include "FaceNormalRender.h"
#include "GameObject.h"
#include "imgui.h"

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

void RenderingComponent::Render()
{
  if (solidRander_ && shader_ == SHADER::DEFFERED)
    SolidRender::draw(meshIndex_, diffuseTexture_, specularTexture_);
}

void RenderingComponent::DebugRender()
{
  if (solidRander_ && shader_ != SHADER::DEFFERED)
  {
    SolidRender::SetShader(shader_);
    SolidRender::draw(meshIndex_, diffuseTexture_, specularTexture_);
  }

  if (vnormRender_) VertexNormalRender::draw(meshIndex_);
  if (fnormRender_) FaceNormalRender::draw(meshIndex_);
}


void RenderingComponent::SetTextures(const Texture& diffuse, const Texture& specular)
{
  diffuseTexture_ = diffuse;
  specularTexture_ = specular;
}

void RenderingComponent::SetDiffuseTexture(const Texture& t)
{
  diffuseTexture_ = t;
}

void RenderingComponent::SetSpecularTexture(const Texture& t)
{
  specularTexture_ = t;
}

void RenderingComponent::Kill()
{
}

void RenderingComponent::ImGuiEditor()
{
  namespace fs = std::filesystem;
  std::vector<char*> objFiles;
  std::vector <fs::path> objFilePaths;
  static char modelDir[256] = "../../Common/models/";
  ImGui::Text("Mesh");
  ImGui::InputText("Directory", modelDir, IM_ARRAYSIZE(modelDir));

  for (const fs::directory_entry& entry : fs::directory_iterator(modelDir))
    if (entry.path().extension() == ".obj")
    {
      std::string nameStr = entry.path().filename().string();
      char* path = new char[nameStr.size() + 1];
      strcpy_s(path, nameStr.size() + 1, nameStr.data());
      objFiles.push_back(path);
      objFilePaths.push_back(entry.path());
    }
  static int currentModel;
  ImGui::PushItemWidth(300);
  ImGui::Combo("Model", &currentModel, objFiles.data(), objFiles.size());
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
    meshIndex_ = SolidRender::loadMesh(obj_meshData, shader_);
    VertexNormalRender::loadMesh(obj_meshData);
    FaceNormalRender::loadMesh(obj_meshData);
  }
  for (char* p : objFiles)
    delete[] p;

  ImGui::Separator();


  ImGui::Checkbox("Render Object", &solidRander_);
  if (solidRander_)
  {
    ImGui::Indent(20);
    ImGui::PushItemWidth(400);

    ImGui::Combo("Shader", reinterpret_cast<int*>(&shader_),
      "PhongLighting\0"
      "PhongShading\0"
      "BlinnLighting\0"
      "BlinnShading\0"
      "FlatEmission\0"
      "Deferred Phong\0"
    );
    if (ImGui::Button("Reload Shaders"))
      SolidRender::ReloadShaders();

    ImGui::PopItemWidth();
    ImGui::Unindent(20);
  }
  ImGui::Checkbox("Render Vertex Normals", &vnormRender_);
  if (vnormRender_)
  {
    ImGui::Indent(20);
    ImGui::PushItemWidth(400);

    static glm::vec4 color = { 1,0,1,1 };
    ImGui::ColorEdit3("color", glm::data(color));
    VertexNormalRender::setRGB(color);

    ImGui::PopItemWidth();
    ImGui::Unindent(20);
  }
  ImGui::Checkbox("Render Face Normals", &fnormRender_);
  if (fnormRender_)
  {
    ImGui::Indent(20);
    ImGui::PushItemWidth(400);

    static glm::vec4 color = { 0,1,1,1 };
    ImGui::ColorEdit3("color", data(color));
    FaceNormalRender::setRGB(color);

    ImGui::PopItemWidth();
    ImGui::Unindent(20);
  }
}

std::string RenderingComponent::Name()
{
  return "Rendering Component";
}
