#include "material.hpp"

#include <file_system.hpp>
#include <logger.hpp>
#include <constants.h>

using kirana::utils::filesystem::getFilename;
using kirana::utils::filesystem::listFilesInPath;
namespace constants = kirana::utils::constants;

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

void kirana::scene::Material::initShaderData()
{

    for (int i = 0; i < static_cast<int>(ShadingPipeline::SHADING_PIPELINE_MAX);
         i++)
    {
        const auto currPipeline = static_cast<ShadingPipeline>(i);
        std::string pipelineFolder = constants::VULKAN_SHADER_DIR_EDITOR_PATH;
        int validPiplineStageStart = static_cast<int>(ShadingStage::VERTEX);
        int validPiplineStageEnd = static_cast<int>(ShadingStage::COMPUTE);
        if (!m_isInternal)
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
        if (m_isInternal)
            break;
    }
}

std::string getMaterialNameFromShader(const std::string &shaderName)
{
    return shaderName + constants::DEFAULT_MATERIAL_NAME_SUFFIX;
}

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE{
        getMaterialNameFromShader(constants::VULKAN_SHADER_EDITOR_OUTLINE_NAME),
        constants::VULKAN_SHADER_EDITOR_OUTLINE_NAME,
        true,
        RasterPipelineData{CullMode::FRONT,
                           SurfaceType::OPAQUE,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true, CompareOperation::NOT_EQUAL,
                            StencilOperation::KEEP, StencilOperation::KEEP,
                            StencilOperation::REPLACE, 1}},
        RaytracePipelineData{},
        DEFAULT_OUTLINE_MATERIAL_PARAMETERS};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_EDITOR_GRID{
        getMaterialNameFromShader(constants::VULKAN_SHADER_EDITOR_GRID_NAME),
        constants::VULKAN_SHADER_EDITOR_GRID_NAME,
        true,
        RasterPipelineData{CullMode::BACK, SurfaceType::TRANSPARENT, true,
                           false},
        RaytracePipelineData{},
        {}};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_BASIC_SHADED{
        getMaterialNameFromShader(constants::VULKAN_SHADER_BASIC_SHADED_NAME),
        constants::VULKAN_SHADER_BASIC_SHADED_NAME,
        false,
        RasterPipelineData{CullMode::BACK,
                           SurfaceType::OPAQUE,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true}},
        RaytracePipelineData{},
        DEFAULT_BASIC_SHADED_MATERIAL_PARAMETERS};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        getMaterialNameFromShader(constants::VULKAN_SHADER_WIREFRAME_NAME),
        constants::VULKAN_SHADER_WIREFRAME_NAME,
        false,
        RasterPipelineData{CullMode::BACK,
                           SurfaceType::WIREFRAME,
                           true,
                           true,
                           CompareOperation::LESS_OR_EQUAL,
                           {true}},
        RaytracePipelineData{},
        DEFAULT_WIREFRAME_MATERIAL_PARAMETERS};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_SHADED{
    getMaterialNameFromShader(constants::VULKAN_SHADER_PRINCIPLED_NAME),
    constants::VULKAN_SHADER_PRINCIPLED_NAME,
    false,
    RasterPipelineData{CullMode::BACK,
                       SurfaceType::OPAQUE,
                       true,
                       true,
                       CompareOperation::LESS_OR_EQUAL,
                       {true}},
    RaytracePipelineData{},
    DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS};