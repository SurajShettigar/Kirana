#include "material_manager.hpp"

#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "shader.hpp"
#include "raster_pipeline.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "push_constant.hpp"
#include "pipeline_layout.hpp"
#include "descriptor_set_layout.hpp"

#include "vulkan_utils.hpp"

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
    else
        return vk::Format::eR32G32B32A32Sfloat;
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
            i, 0,
            getFormatFromVertexAttribInfo(rasterData.vertexAttributeInfo[i]),
            static_cast<uint32_t>(
                rasterData.vertexAttributeInfo[i].structOffset)};
    }
    return desc;
}

int kirana::viewport::vulkan::MaterialManager::createShader(
    const scene::ShaderData &shaderData)
{
    auto it = std::find_if(m_shaders.begin(), m_shaders.end(),
                           [&shaderData](const Shader *&shader) {
                               return shader->name == shaderData.name &&
                                      shader->shadingPipeline ==
                                          static_cast<vulkan::ShadingPipeline>(
                                              shaderData.pipeline);
                           });
    if (it != m_shaders.end())
        return static_cast<int>(it - m_shaders.begin());

    const Shader *shader = new Shader(m_device, shaderData);
    if (!shader->isInitialized)
    {
        delete shader;
        return -1;
    }
    m_shaders.emplace_back(std::move(shader));
    return static_cast<int>(m_shaders.size() - 1);
}

int kirana::viewport::vulkan::MaterialManager::createPipeline(
    vulkan::ShadingPipeline shadingPipeline, const RenderPass &renderPass,
    const Shader *const shader, const scene::Material &material)
{
    auto it =
        std::find_if(m_pipelines.begin(), m_pipelines.end(),
                     [&shadingPipeline, &material](const Pipeline *&pipeline) {
                         return pipeline->name == material.getShaderName() &&
                                pipeline->shadingPipeline == shadingPipeline;
                     });

    if (it != m_pipelines.end())
        return static_cast<int>(it - m_pipelines.begin());

    switch (shadingPipeline)
    {
    case ShadingPipeline::RAYTRACE: {
        const auto &raytraceData = material.getRaytracePipelineData();
        const RaytracePipeline::Properties props{
            raytraceData.maxRecursionDepth};
        const RaytracePipeline *pipeline =
            new RaytracePipeline(m_device, &renderPass, shader, props);
        if (!pipeline->isInitialized)
        {
            delete pipeline;
            return -1;
        }
        m_pipelines.emplace_back(std::move(pipeline));
    }
    break;
    default:
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
        const RasterPipeline *pipeline =
            new RasterPipeline(m_device, &renderPass, shader, props);
        if (!pipeline->isInitialized)
        {
            delete pipeline;
            return -1;
        }
        m_pipelines.emplace_back(std::move(pipeline));
    }
    break;
    }
    return static_cast<int>(m_pipelines.size() - 1);
}


int kirana::viewport::vulkan::MaterialManager::copyMaterialDataToBuffer(
    const scene::Material &material)
{
    const auto &shaderName = material.getShaderName();
    auto &shaderDataBuffers = m_materialDataBuffers[shaderName];

    std::vector<uint8_t> matData;
    material.getParametersData(&matData);

    const size_t dataSize = matData.size();

    if (dataSize == 0)
        return -1;

    bool sizeExceeded =
        !shaderDataBuffers.empty() &&
        (dataSize + shaderDataBuffers.back().currentSize) >
            constants::VULKAN_MATERIAL_DATA_BUFFER_BATCH_SIZE_LIMIT;

    if (shaderDataBuffers.empty() || sizeExceeded)
    {
        const size_t bufferSize = std::max(
            dataSize, constants::VULKAN_MATERIAL_DATA_BUFFER_BATCH_SIZE_LIMIT);

        BatchBufferData buffer{};
        if (!m_allocator->allocateBuffer(
                &buffer.buffer, bufferSize,
                vk::BufferUsageFlagBits::eStorageBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress,
                Allocator::AllocationType::WRITEABLE))
        {
            Logger::get().log(
                constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                "Failed to allocate material data buffer for shader: " +
                    shaderName);
            return -1;
        }
        m_device->setDebugObjectName(
            *buffer.buffer.buffer,
            "MaterialDataBuffer_" + shaderName + "_" +
                std::to_string(shaderDataBuffers.size()));
        buffer.currentSize = 0;
        buffer.currentDataCount = 0;
        shaderDataBuffers.emplace_back(std::move(buffer));
    }



    auto &buffer = shaderDataBuffers.back();
    m_allocator->copyDataToBuffer(buffer.buffer, matData.data(),
                                  buffer.currentSize, dataSize);
    buffer.currentSize += dataSize;
    buffer.currentDataCount++;
    return static_cast<int>(shaderDataBuffers.size() - 1);
}

int kirana::viewport::vulkan::MaterialManager::createSBT(
    const RaytracePipeline *pipeline)
{
    auto it = std::find_if(m_SBTs.begin(), m_SBTs.end(),
                           [&pipeline](const ShaderBindingTable *&sbt) {
                               return sbt->name == pipeline->name;
                           });

    if (it != m_SBTs.end())
        return static_cast<int>(it - m_SBTs.begin());

    const ShaderBindingTable *sbt =
        new ShaderBindingTable(m_device, m_allocator, pipeline);
    if (!sbt->isInitialized)
    {
        delete sbt;
        return -1;
    }
    m_SBTs.emplace_back(sbt);
    return static_cast<int>(m_SBTs.size() - 1);
}


void kirana::viewport::vulkan::MaterialManager::onMaterialChanged(
    const std::string &materialName,
    const scene::MaterialProperties &properties, const std::string &param,
    const std::any &value)
{
    const Material &mat = m_materials[m_materialIndexTable.at(materialName)];
    if (mat.dataBufferIndex > -1 && mat.materialDataIndex > -1)
    {
        auto &buffer = m_materialDataBuffers.at(
            m_materialShaderTable[materialName])[mat.dataBufferIndex];

        std::vector<uint8_t> matData;
        properties.getParametersData(&matData);
        const size_t dataSize = matData.size();

        // TODO: Copy only changed parameter instead of copying entire values.
        m_allocator->copyDataToBuffer(buffer.buffer, matData.data(),
                                      mat.materialDataIndex * dataSize,
                                      dataSize);
    }
}

kirana::viewport::vulkan::MaterialManager::MaterialManager(
    const Device *const device, const Allocator *const allocator)
    : m_device{device}, m_allocator{allocator}
{
}

kirana::viewport::vulkan::MaterialManager::~MaterialManager()
{
    for (auto &m : m_materialDataBuffers)
        for (auto &b : m.second)
            m_allocator->free(b.buffer);
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

uint32_t kirana::viewport::vulkan::MaterialManager::addMaterial(
    const RenderPass &renderPass, const scene::Material &material)
{
    if (m_materialIndexTable.find(material.getName()) !=
        m_materialIndexTable.end())
        return static_cast<int>(m_materialIndexTable.at(material.getName()));

    const std::string &shaderName = material.getShaderName();
    const std::string &materialName = material.getName();

    m_materialShaderTable[materialName] = material.getShaderName();

    Material m{};
    m.dataBufferIndex = copyMaterialDataToBuffer(material);
    m.materialDataIndex =
        m.dataBufferIndex > -1
            ? static_cast<int>(
                  m_materialDataBuffers.at(shaderName)[m.dataBufferIndex]
                      .currentDataCount -
                  1)
            : -1;

    m.shaderIndices.resize(
        static_cast<int>(vulkan::ShadingPipeline::SHADING_MAX));
    m.pipelineIndices.resize(
        static_cast<int>(vulkan::ShadingPipeline::SHADING_MAX));
    for (int i = 0; i < static_cast<int>(vulkan::ShadingPipeline::SHADING_MAX);
         i++)
    {
        m.shaderIndices[i] = -1;
        m.pipelineIndices[i] = -1;
        m.sbtIndex = -1;

        const scene::ShaderData &shaderData =
            material.getShaderData(static_cast<scene::ShadingPipeline>(i));

        if (shaderData.stages.empty())
            continue;

        const auto shadingP =
            static_cast<vulkan::ShadingPipeline>(shaderData.pipeline);

        m.shaderIndices[i] = createShader(shaderData);
        m.pipelineIndices[i] =
            m.shaderIndices[i] > -1
                ? createPipeline(shadingP, renderPass,
                                 m_shaders[m.shaderIndices[i]], material)
                : -1;

        if (shadingP == ShadingPipeline::RAYTRACE)
        {
            m.sbtIndex =
                m.pipelineIndices[i] > -1
                    ? createSBT(reinterpret_cast<const RaytracePipeline *>(
                          m_pipelines[m.pipelineIndices[i]]))
                    : -1;
        }
    }
    m.materialChangeListener = material.addOnParameterChangeEventListener(
        [&](const scene::MaterialProperties &properties,
            const std::string &param, const std::any &value) {
            onMaterialChanged(materialName, properties, param, value);
        });

    m_materials.emplace_back(m);
    m_materialIndexTable[materialName] =
        static_cast<uint32_t>(m_materials.size() - 1);
    return m_materialIndexTable[materialName];
}

vk::DeviceAddress kirana::viewport::vulkan::MaterialManager::
    getMaterialDataBufferAddress(uint32_t materialIndex) const
{
    const Material &mat = m_materials[materialIndex];

    if (mat.dataBufferIndex < 0 || mat.materialDataIndex < 0 ||
        mat.shaderIndices.empty() || mat.shaderIndices[0] < 0)
        return 0;

    const auto &shader = m_shaders[mat.shaderIndices[0]];
    const auto &buffer =
        m_materialDataBuffers.at(shader->name)[mat.dataBufferIndex];

    return buffer.buffer.address;
}
