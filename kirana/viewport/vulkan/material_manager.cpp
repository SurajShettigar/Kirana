#include "material_manager.hpp"

#include "device.hpp"
#include "shader.hpp"
#include "raster_pipeline.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "push_constant.hpp"

#include <material.hpp>

#include <algorithm>

vk::Format kirana::viewport::vulkan::MaterialManager::
    getFormatFromVertexAttribInfo(const scene::VertexInfo &info)
{
    if (info.componentCount == 1)
    {
        switch (info.format)
        {
        case scene::VertexDataFormat::INT:
            return vk::Format::eR32Sint;
        case scene::VertexDataFormat::FLOAT:
            return vk::Format::eR32Sfloat;
        }
    }
    else if (info.componentCount == 2)
    {
        switch (info.format)
        {
        case scene::VertexDataFormat::INT:
            return vk::Format::eR32G32Sint;
        case scene::VertexDataFormat::FLOAT:
            return vk::Format::eR32G32Sfloat;
        }
    }
    else if (info.componentCount == 3)
    {
        switch (info.format)
        {
        case scene::VertexDataFormat::INT:
            return vk::Format::eR32G32B32Sint;
        case scene::VertexDataFormat::FLOAT:
            return vk::Format::eR32G32B32Sfloat;
        }
    }
    else if (info.componentCount == 4)
    {
        switch (info.format)
        {
        case scene::VertexDataFormat::INT:
            return vk::Format::eR32G32B32A32Sint;
        case scene::VertexDataFormat::FLOAT:
            return vk::Format::eR32G32B32A32Sfloat;
        }
    }
}

kirana::viewport::vulkan::MaterialManager::VertexInputDescription kirana::
    viewport::vulkan::MaterialManager::getVertexInputDescription(
        const scene::RasterPipelineData &rasterData)
{
    VertexInputDescription desc{};
    desc.bindings = {{0, sizeof(scene::Vertex), vk::VertexInputRate::eVertex}};
    desc.attributes.resize(rasterData.vertexAttributeInfo.size());

    for (uint32_t i = 0; i < rasterData.vertexAttributeInfo.size(); i++)
    {
        desc.attributes[i] = vk::VertexInputAttributeDescription{
            0, i,
            getFormatFromVertexAttribInfo(rasterData.vertexAttributeInfo[i]),
            static_cast<uint32_t>(
                rasterData.vertexAttributeInfo[i].structOffset)};
    }
    return desc;
}

const kirana::viewport::vulkan::Shader *kirana::viewport::vulkan::
    MaterialManager::createShader(const scene::ShaderData &shaderData)
{
    auto it = std::find_if(m_shaders.begin(), m_shaders.end(),
                           [&shaderData](const Shader *&shader) {
                               return shader->name == shaderData.name &&
                                      shader->shadingPipeline ==
                                          static_cast<vulkan::ShadingPipeline>(
                                              shaderData.pipeline);
                           });
    if (it != m_shaders.end())
        return *it;

    m_shaders.emplace_back(std::move(new Shader(m_device, shaderData)));
    return m_shaders.back();
}

const kirana::viewport::vulkan::Pipeline *kirana::viewport::vulkan::
    MaterialManager::createPipeline(vulkan::ShadingPipeline shadingPipeline,
                                    const RenderPass &renderPass,
                                    const Shader *const shader,
                                    const scene::Material &material)
{
    auto it =
        std::find_if(m_pipelines.begin(), m_pipelines.end(),
                     [&shadingPipeline, &material](const Pipeline *&pipeline) {
                         return pipeline->name == material.getShaderName() &&
                                pipeline->shadingPipeline == shadingPipeline;
                     });

    if (it != m_pipelines.end())
        return *it;

    switch (shadingPipeline)
    {
    case ShadingPipeline::RASTER: {
        const auto &rasterData = material.getRasterPipelineData();
        const auto &vertexData = getVertexInputDescription(rasterData);
        const RasterPipeline::Properties props{
            vertexData.bindings,
            vertexData.attributes,
            vk::PrimitiveTopology::eTriangleList,
            rasterData.surfaceType == scene::SurfaceType::WIREFRAME
                ? vk::PolygonMode::eLine
                : vk::PolygonMode::eFill,
            vk::CullModeFlags(
                static_cast<vk::CullModeFlagBits>(rasterData.cull)),
            1.0f,
            vk::SampleCountFlagBits::e1,
            rasterData.surfaceType == scene::SurfaceType::TRANSPARENT,
            rasterData.enableDepth,
            rasterData.writeDepth,
            static_cast<vk::CompareOp>(rasterData.depthCompareOp),
            rasterData.stencil.enableTest,
            static_cast<vk::CompareOp>(rasterData.stencil.compareOp),
            static_cast<vk::StencilOp>(rasterData.stencil.failOp),
            static_cast<vk::StencilOp>(rasterData.stencil.depthFailOp),
            static_cast<vk::StencilOp>(rasterData.stencil.passOp),
            rasterData.stencil.reference};
        m_pipelines.emplace_back(
            new RasterPipeline(m_device, &renderPass, shader, props));
    }
    break;
    case ShadingPipeline::RAYTRACE: {
        const auto &raytraceData = material.getRaytracePipelineData();
        const RaytracePipeline::Properties props{
            raytraceData.maxRecursionDepth};
        m_pipelines.emplace_back(
            new RaytracePipeline(m_device, &renderPass, shader, props));
    }
    break;
    }
    return m_pipelines.back();
}

const kirana::viewport::vulkan::ShaderBindingTable *kirana::viewport::vulkan::
    MaterialManager::createSBT(const RaytracePipeline *pipeline)
{
    auto it = std::find_if(m_SBTs.begin(), m_SBTs.end(),
                           [&pipeline](const ShaderBindingTable *&sbt) {
                               return sbt->name == pipeline->name;
                           });

    if (it != m_SBTs.end())
        return *it;

    m_SBTs.emplace_back(
        new ShaderBindingTable(m_device, m_allocator, pipeline));
    return m_SBTs.back();
}

kirana::viewport::vulkan::MaterialManager::MaterialManager(
    const Device *const device, const Allocator *const allocator)
    : m_device{device}, m_allocator{allocator}
{
}

kirana::viewport::vulkan::MaterialManager::~MaterialManager()
{
    for (auto &s : m_SBTs)
    {
        if (s)
        {
            delete s;
            s = nullptr;
        }
    }
    for (auto &p : m_pipelines)
    {
        if (p)
        {
            delete p;
            p = nullptr;
        }
    }
    for (auto &s : m_shaders)
    {
        if (s)
        {
            delete s;
            s = nullptr;
        }
    }
}


bool kirana::viewport::vulkan::MaterialManager::addMaterial(
    const RenderPass &renderPass, const scene::Material &material)
{
    for (int i = 0; i < static_cast<int>(vulkan::ShadingPipeline::SHADING_MAX);
         i++)
    {
        const scene::ShaderData &shaderData =
            material.getShaderData(static_cast<scene::ShadingPipeline>(i));
        const auto shadingP =
            static_cast<vulkan::ShadingPipeline>(shaderData.pipeline);

        Material m{};
        m.shader = createShader(shaderData);
        if (!m.shader->isInitialized)
            return false;
        m.pipeline = createPipeline(shadingP, renderPass, m.shader, material);
        if (!m.pipeline->isInitialized)
            return false;
        if (shadingP == vulkan::ShadingPipeline::RASTER)
        {
            RasterMaterial &rm = m_rasterMaterials[material.getName()];
            rm.pipeline = m.pipeline;
            rm.shader = m.shader;
            rm.pushConstantData = new PushConstant<vulkan::PushConstantRaster>(
                {}, rm.shader->getPushConstant()->getRange().stageFlags);
        }
        else if (shadingP == vulkan::ShadingPipeline::RAYTRACE)
        {
            RaytraceMaterial &rm = m_raytraceMaterials[material.getName()];
            rm.pipeline = m.pipeline;
            rm.shader = m.shader;
            rm.sbt = createSBT(
                reinterpret_cast<const RaytracePipeline *>(rm.pipeline));
            rm.pushConstantData = new PushConstant<vulkan::PushConstantRaytrace>(
                {}, rm.shader->getPushConstant()->getRange().stageFlags);
        }
    }
    return true;
}