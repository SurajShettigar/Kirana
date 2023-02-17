#include "material.hpp"

#include <constants.h>
#include <file_system.hpp>

#include <random>
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

    for (int i = 0; i < static_cast<int>(ShadingPipeline::SHADING_PIPELINE_MAX);
         i++)
    {
        const auto currPipeline = static_cast<ShadingPipeline>(i);
        std::string pipelineFolder = constants::VULKAN_SHADER_DIR_EDITOR_PATH;
        int validPiplineStageStart = static_cast<int>(ShadingStage::VERTEX);
        int validPiplineStageEnd = static_cast<int>(ShadingStage::COMPUTE);
        if (!m_isEditorMaterial)
        {
            switch (currPipeline)
            {
            case ShadingPipeline::RASTER:
                pipelineFolder = constants::VULKAN_SHADER_DIR_RASTER_PATH;
                validPiplineStageStart = static_cast<int>(ShadingStage::VERTEX);
                validPiplineStageEnd = static_cast<int>(ShadingStage::COMPUTE);
                break;
            case ShadingPipeline::RAYTRACE:
                pipelineFolder = constants::VULKAN_SHADER_DIR_RAYTRACE_PATH;
                validPiplineStageStart =
                    static_cast<int>(ShadingStage::RAY_GEN);
                validPiplineStageEnd = static_cast<int>(ShadingStage::CALLABLE);
                break;
            default:
                pipelineFolder = constants::VULKAN_SHADER_DIR_RASTER_PATH;
                break;
            }
        }
        const std::string basePath = utils::filesystem::combinePath(
            constants::VULKAN_SHADER_DIR_ROOT_PATH, {pipelineFolder.c_str()},
            "");
        ShaderData shaderData{};
        shaderData.name = m_shaderName;
        shaderData.pipeline = currPipeline;
        shaderData.stages.clear();
        for (const auto &f : listFilesInPath(basePath, m_shaderName))
        {
            const std::string extension = getFilename(f, false).second;

            if (SHADING_EXTENSION_STAGE_TABLE.find(extension) !=
                SHADING_EXTENSION_STAGE_TABLE.end())
            {
                const auto &stage = SHADING_EXTENSION_STAGE_TABLE.at(extension);
                const int stageValue = static_cast<int>(stage);
                // Make sure the stage is valid for the current shading
                // pipeline.
                if (stageValue >= validPiplineStageStart &&
                    stageValue <= validPiplineStageEnd)
                    shaderData.stages[stage].emplace_back(f);
            }
        }
        m_shaderData[i] = shaderData;
        if (m_isEditorMaterial)
            break;
    }
}

static std::default_random_engine randomEngine;

kirana::scene::Material::Material()
    : m_shaderName{constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME},
      m_name{getMaterialNameFromShaderName(m_shaderName)},
      m_properties{MaterialProperties{
          RasterPipelineData{CullMode::BACK,
                             SurfaceType::OPAQUE,
                             true,
                             true,
                             CompareOperation::LESS_OR_EQUAL,
                             {true}},
          RaytracePipelineData{}, DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS}},
      m_isEditorMaterial{false}
{
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> dist(0.5f, 1.0f);
    m_properties.parameters[0].value = math::Vector4(
        colorDist(randomEngine), colorDist(randomEngine), colorDist(randomEngine), 1.0f);
    m_properties.parameters[5].value = dist(randomEngine);

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
            RasterPipelineData{CullMode::FRONT,
                               SurfaceType::OPAQUE,
                               true,
                               true,
                               CompareOperation::LESS_OR_EQUAL,
                               {true, CompareOperation::NOT_EQUAL,
                                StencilOperation::KEEP, StencilOperation::KEEP,
                                StencilOperation::REPLACE, 1}},
            RaytracePipelineData{}, DEFAULT_OUTLINE_MATERIAL_PARAMETERS},
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_GRID{
        constants::VULKAN_SHADER_EDITOR_GRID_NAME, "",
        MaterialProperties{RasterPipelineData{CullMode::BACK,
                                              SurfaceType::TRANSPARENT, true,
                                              false},
                           RaytracePipelineData{},
                           {}},
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_BASIC_SHADED{
        constants::VULKAN_SHADER_BASIC_SHADED_NAME, "",
        MaterialProperties{RasterPipelineData{CullMode::BACK,
                                              SurfaceType::OPAQUE,
                                              true,
                                              true,
                                              CompareOperation::LESS_OR_EQUAL,
                                              {true}},
                           RaytracePipelineData{},
                           DEFAULT_BASIC_SHADED_MATERIAL_PARAMETERS}};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        getMaterialNameFromShaderName(constants::VULKAN_SHADER_WIREFRAME_NAME),
        constants::VULKAN_SHADER_WIREFRAME_NAME,
        MaterialProperties{RasterPipelineData{CullMode::BACK,
                                              SurfaceType::WIREFRAME,
                                              true,
                                              true,
                                              CompareOperation::LESS_OR_EQUAL,
                                              {true}},
                           RaytracePipelineData{},
                           DEFAULT_WIREFRAME_MATERIAL_PARAMETERS}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_SHADED{
    constants::VULKAN_SHADER_PRINCIPLED_NAME, "",
    MaterialProperties{RasterPipelineData{CullMode::BACK,
                                          SurfaceType::OPAQUE,
                                          true,
                                          true,
                                          CompareOperation::LESS_OR_EQUAL,
                                          {true}},
                       RaytracePipelineData{},
                       DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS}};