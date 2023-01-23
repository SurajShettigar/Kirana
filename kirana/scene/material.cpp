#include "material.hpp"

#include <constants.h>
#include <file_system.hpp>

#include <utility>

namespace constants = kirana::utils::constants;

std::string kirana::scene::Material::getMaterialNameFromShaderName(
    const std::string &shaderName)
{
    return shaderName + constants::DEFAULT_MATERIAL_NAME_SUFFIX;
}

void kirana::scene::Material::setShaderData()
{
    using utils::filesystem::getFilename;
    using utils::filesystem::listFilesInPath;
    std::string pipelineFolder = m_isEditorMaterial
                                     ? constants::VULKAN_SHADER_DIR_EDITOR_PATH
                                     : constants::VULKAN_SHADER_DIR_RASTER_PATH;
    std::string basePath = utils::filesystem::combinePath(
        constants::VULKAN_SHADER_DIR_ROOT_PATH, {pipelineFolder.c_str()}, "");
    ShaderData shaderData{};
    shaderData.name = m_shaderName;
    shaderData.pipeline = ShadingPipeline::RASTER;

    shaderData.stages.clear();
    auto addStages = [&]() {
        for (const auto &f : listFilesInPath(basePath, shaderData.name))
        {
            const auto &extension = getFilename(f).second;
            if (SHADING_EXTENSION_STAGE_TABLE.find(extension) !=
                SHADING_EXTENSION_STAGE_TABLE.end())
            {
                const auto &stage = SHADING_EXTENSION_STAGE_TABLE.at(extension);
                shaderData.stages[stage] = f;
            }
        }
    };
    addStages();
    m_shaderData[0] = shaderData;

    // Add raytraced material stages, if available
    if (!m_isEditorMaterial)
    {
        pipelineFolder = constants::VULKAN_SHADER_DIR_RAYTRACE_PATH;
        basePath = utils::filesystem::combinePath(
            constants::VULKAN_SHADER_DIR_ROOT_PATH, {pipelineFolder.c_str()},
            "");
        shaderData.pipeline = ShadingPipeline::RAYTRACE;
        shaderData.stages.clear();
        addStages();
        m_shaderData[1] = shaderData;
    }
}

kirana::scene::Material::Material()
    : m_shaderName{constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME},
      m_name{getMaterialNameFromShaderName(m_shaderName)},
      m_properties{
          MaterialProperties{{{}, CullMode::BACK, SurfaceType::WIREFRAME},
                             {},
                             std::make_unique<WireframeMaterialData>(
                                 DEFAULT_WIREFRAME_MATERIAL_DATA)}},
      m_isEditorMaterial{false}
{
    setShaderData();
}

kirana::scene::Material::Material(std::string shaderName,
                                  std::string materialName,
                                  MaterialProperties properties,
                                  bool isEditorMaterial)
    : m_shaderName{shaderName.empty()
                       ? constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME
                       : std::move(shaderName)},
      m_name{materialName.empty() ? getMaterialNameFromShaderName(m_shaderName)
                                  : std::move(materialName)},
      m_properties{std::move(properties)}, m_isEditorMaterial{isEditorMaterial}
{
    setShaderData();
}

kirana::scene::Material::Material(const Material &material)
{
    if (this != &material)
    {
        m_shaderName = material.m_shaderName;
        m_name = material.m_name;
        m_properties = material.m_properties;
        m_isEditorMaterial = material.m_isEditorMaterial;
        m_shaderData = material.m_shaderData;
    }
}

kirana::scene::Material &kirana::scene::Material::operator=(
    const Material &material)
{
    if (this != &material)
    {
        m_shaderName = material.m_shaderName;
        m_name = material.m_name;
        m_properties = material.m_properties;
        m_isEditorMaterial = material.m_isEditorMaterial;
        m_shaderData = material.m_shaderData;
    }
    return *this;
}

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE{
        constants::VULKAN_SHADER_EDITOR_OUTLINE_NAME, "",
        MaterialProperties{
            {{},
             CullMode::FRONT,
             SurfaceType::OPAQUE,
             false,
             true,
             CompareOperation::LESS_OR_EQUAL,
             {true, CompareOperation::NOT_EQUAL, StencilOperation::KEEP,
              StencilOperation::KEEP, StencilOperation::REPLACE, 1}},
            {},
            std::make_unique<OutlineMaterialData>(
                DEFAULT_OUTLINE_MATERIAL_DATA)},
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_GRID{
        constants::VULKAN_SHADER_EDITOR_GRID_NAME, "",
        MaterialProperties{
            {{}, CullMode::BACK, SurfaceType::TRANSPARENT, true, false},
            {},
            nullptr},
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_BASIC_SHADED{
        constants::VULKAN_SHADER_BASIC_SHADED_NAME, "",
        MaterialProperties{{},
                           {},
                           std::make_unique<BasicShadedMaterialData>(
                               DEFAULT_BASIC_SHADED_MATERIAL_DATA)}};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        getMaterialNameFromShaderName(constants::VULKAN_SHADER_WIREFRAME_NAME),
        constants::VULKAN_SHADER_WIREFRAME_NAME,
        MaterialProperties{{{}, CullMode::BACK, SurfaceType::WIREFRAME},
                           {},
                           std::make_unique<WireframeMaterialData>(
                               DEFAULT_WIREFRAME_MATERIAL_DATA)}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_SHADED{
    constants::VULKAN_SHADER_PRINCIPLED_NAME, "",
    MaterialProperties{{},
                       {},
                       std::make_unique<PrincipledMaterialData>(
                           DEFAULT_PRINCIPLED_MATERIAL_DATA)}};