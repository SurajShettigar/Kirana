#include "material.hpp"

#include <constants.h>
#include <file_system.hpp>
#include <math_utils.hpp>

#include <random>
#include <utility>
#include <assimp/material.h>

namespace constants = kirana::utils::constants;

std::string kirana::scene::Material::getMaterialNameFromShaderName(
    const std::string &shaderName)
{
    return shaderName + constants::DEFAULT_MATERIAL_NAME_SUFFIX;
}

kirana::scene::ShadingStage kirana::scene::Material::getShadingStage(
    const std::string &filePath) const
{
    const std::string extension =
        utils::filesystem::getFilename(filePath, false).second;
    for (const auto &s : SHADING_EXTENSION_STAGE_TABLE)
    {
        if (extension.find(s.first) != std::string::npos)
            return SHADING_EXTENSION_STAGE_TABLE.at(s.first);
    }
    return ShadingStage::NONE;
}

void kirana::scene::Material::setShaderData()
{
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
            const ShadingStage stage = getShadingStage(f);
            if (stage != ShadingStage::NONE)
            {
                const int stageValue = static_cast<int>(stage);
                // Make sure the stage is valid for the current shading
                // pipeline.
                if (stageValue >= validPiplineStageStart &&
                    stageValue <= validPiplineStageEnd)
                    shaderData.stages[stage].emplace_back(f);
            }
        }
        m_shaderData.at(i) = shaderData;
        if (m_isEditorMaterial)
            break;
    }
}

void kirana::scene::Material::setParametersFromAiMaterial(
    const aiMaterial *material)
{
    // TODO: Use constant parameter names.

    aiColor3D diffuseColor{1.0f, 1.0f, 1.0f};
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    setParameter("_BaseColor", math::Vector4(diffuseColor.r, diffuseColor.g,
                                             diffuseColor.b, 1.0f));

    aiColor3D emissiveColor{0.0f, 0.0f, 0.0f};
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    setParameter(
        "_EmissiveColor",
        math::Vector4(emissiveColor.r, emissiveColor.g, emissiveColor.b, 1.0f));

    float opacity{1.0f};
    material->Get(AI_MATKEY_OPACITY, opacity);
    setParameter("_Transmission", opacity);

    float gloss{0.0f};
    material->Get(AI_MATKEY_SHININESS, gloss);
    setParameter("_Roughness", math::clampf(1.0f - gloss, 0.0f, 1.0f));
    float ior{1.0f};
    material->Get(AI_MATKEY_REFRACTI, ior);
    setParameter("_Ior", ior);
}

static std::default_random_engine randomEngine;

kirana::scene::Material::Material()
    : MaterialProperties{RasterPipelineData{CullMode::BACK,
                                            SurfaceType::OPAQUE,
                                            true,
                                            true,
                                            CompareOperation::LESS_OR_EQUAL,
                                            {true}},
                         RaytracePipelineData{},
                         DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS},
      m_shaderName{constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME},
      m_name{getMaterialNameFromShaderName(m_shaderName)}, m_isEditorMaterial{
                                                               false}
{
    const std::uniform_real_distribution<float> colorDist(0.75f, 1.0f);
    const std::uniform_real_distribution<float> dist(0.5f, 1.0f);
    m_parameters[0].value =
        math::Vector4(colorDist(randomEngine), colorDist(randomEngine),
                      colorDist(randomEngine), 1.0f);
    m_parameters[2].value = 0.0f;
    m_parameters[5].value = 0.75f;

    setShaderData();
}

kirana::scene::Material::Material(const aiMaterial *material)
    : MaterialProperties{RasterPipelineData{CullMode::BACK,
                                            SurfaceType::OPAQUE,
                                            true,
                                            true,
                                            CompareOperation::LESS_OR_EQUAL,
                                            {true}},
                         RaytracePipelineData{},
                         DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS},
      m_shaderName{constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME},
      m_name{material->GetName().C_Str()}, m_isEditorMaterial{false}
{
    setShaderData();
    setParametersFromAiMaterial(material);
}

kirana::scene::Material::Material(
    std::string shaderName, std::string materialName,
    const RasterPipelineData &rasterData,
    const RaytracePipelineData &raytraceData,
    const std::vector<MaterialParameter> &parameters, bool isEditorMaterial)
    : MaterialProperties{rasterData, raytraceData, parameters},
      m_shaderName{shaderName.empty()
                       ? constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME
                       : std::move(shaderName)},
      m_name{materialName.empty() ? getMaterialNameFromShaderName(m_shaderName)
                                  : std::move(materialName)},
      m_isEditorMaterial{isEditorMaterial}
{
    setShaderData();
}

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE{
        constants::VULKAN_SHADER_EDITOR_OUTLINE_NAME,
        "",
        RasterPipelineData{CullMode::FRONT,
                           SurfaceType::OPAQUE,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true, CompareOperation::NOT_EQUAL,
                            StencilOperation::KEEP, StencilOperation::KEEP,
                            StencilOperation::REPLACE, 1}},
        RaytracePipelineData{},
        DEFAULT_OUTLINE_MATERIAL_PARAMETERS,
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_GRID{
        constants::VULKAN_SHADER_EDITOR_GRID_NAME,
        "",
        RasterPipelineData{CullMode::BACK, SurfaceType::TRANSPARENT, true,
                           false},
        RaytracePipelineData{},
        {},
        true};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_BASIC_SHADED{
        constants::VULKAN_SHADER_BASIC_SHADED_NAME, "",
        RasterPipelineData{CullMode::BACK,
                           SurfaceType::OPAQUE,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true}},
        RaytracePipelineData{}, DEFAULT_BASIC_SHADED_MATERIAL_PARAMETERS};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        getMaterialNameFromShaderName(constants::VULKAN_SHADER_WIREFRAME_NAME),
        constants::VULKAN_SHADER_WIREFRAME_NAME,
        RasterPipelineData{CullMode::BACK,
                           SurfaceType::WIREFRAME,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true}},
        RaytracePipelineData{}, DEFAULT_WIREFRAME_MATERIAL_PARAMETERS};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_SHADED{
    constants::VULKAN_SHADER_PRINCIPLED_NAME, "",
    RasterPipelineData{CullMode::BACK,
                       SurfaceType::OPAQUE,
                       true,
                       true,
                       CompareOperation::LESS_OR_EQUAL,
                       {true}},
    RaytracePipelineData{}, DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS};