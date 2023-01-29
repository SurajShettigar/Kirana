#include "scene_data.hpp"
#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"
#include "allocator.hpp"
#include "shader.hpp"
#include "descriptor_set_layout.hpp"
#include "pipeline_layout.hpp"
#include "raster_pipeline.hpp"
#include "acceleration_structure.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "push_constant.hpp"

#include <algorithm>
#include <viewport_scene.hpp>

void kirana::viewport::vulkan::SceneData::setVertexDescription()
{
    const vk::VertexInputBindingDescription bindingDesc(0,
                                                        sizeof(scene::Vertex));
    m_vertexDesc.bindings.push_back(bindingDesc);

    const vk::VertexInputAttributeDescription posAttrib(
        0, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, position));
    const vk::VertexInputAttributeDescription normalAttrib(
        1, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, normal));
    const vk::VertexInputAttributeDescription colorAttrib(
        2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(scene::Vertex, color));

    m_vertexDesc.attributes.push_back(posAttrib);
    m_vertexDesc.attributes.push_back(normalAttrib);
    m_vertexDesc.attributes.push_back(colorAttrib);
}

void kirana::viewport::vulkan::SceneData::onWorldChanged()
{
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::WorldData));

    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToMemory(
            m_worldDataBuffer, sizeof(scene::WorldData),
            static_cast<uint32_t>(paddedSize * i), &m_scene.getWorldData());
    }
    updateRaytracedFrameCount(true);
}

void kirana::viewport::vulkan::SceneData::onCameraChanged()
{
    const auto &cameraData = m_scene.getCameraData();
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::CameraData));
    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToMemory(m_cameraBuffer, sizeof(scene::CameraData),
                                      static_cast<uint32_t>(paddedSize * i),
                                      &cameraData);
    }
    updateRaytracedFrameCount(true);
}

void kirana::viewport::vulkan::SceneData::onSceneLoaded(bool result)
{
}

void kirana::viewport::vulkan::SceneData::onObjectChanged()
{
    auto data =
        reinterpret_cast<vulkan::ObjectData *>(m_objectBuffer.memoryPointer);
    size_t index = 0;
    for (size_t frameIndex = 0;
         frameIndex < constants::VULKAN_FRAME_OVERLAP_COUNT; frameIndex++)
    {
        for (const auto &m : m_meshes)
        {
            for (const auto &i : m.instances)
            {
                data[index].modelMatrix = i.transform->getMatrix();
                ++index;
            }
        }
    }
    updateRaytracedFrameCount(true);
}

void kirana::viewport::vulkan::SceneData::createWorldDataBuffer()
{
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::WorldData));
    const vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
            vma::MemoryUsage::eCpuToGpu, &m_worldDataBuffer))
    {
        m_worldDataBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_worldDataBuffer.buffer, 0, sizeof(scene::WorldData));

        m_device->setDebugObjectName(*m_cameraBuffer.buffer, "WorldBuffer");
        onWorldChanged();
    }
}

void kirana::viewport::vulkan::SceneData::createCameraBuffer()
{
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::CameraData));
    const vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
            vma::MemoryUsage::eCpuToGpu, &m_cameraBuffer))
    {
        m_cameraBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_cameraBuffer.buffer, 0, sizeof(scene::CameraData));

        m_device->setDebugObjectName(*m_cameraBuffer.buffer, "CameraBuffer");
        onCameraChanged();
    }
}

bool kirana::viewport::vulkan::SceneData::createVertexAndIndexBuffer(
    const std::vector<scene::Vertex> &vertices,
    const std::vector<uint32_t> &indices)
{
    const size_t verticesSize = vertices.size() * sizeof(scene::Vertex);
    if (!m_allocator->allocateBufferToGPU(
            verticesSize,
            vk::BufferUsageFlagBits::eVertexBuffer |
                vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::
                    eAccelerationStructureBuildInputReadOnlyKHR |
                vk::BufferUsageFlagBits::eStorageBuffer,
            &m_vertexBuffer, vertices.data()))
        return false;

    m_device->setDebugObjectName(*m_vertexBuffer.buffer, "VertexBuffer");
    m_vertexBuffer.descInfo =
        vk::DescriptorBufferInfo(*m_vertexBuffer.buffer, 0, verticesSize);

    const size_t indicesSize = indices.size() * sizeof(uint32_t);
    if (!m_allocator->allocateBufferToGPU(
            indicesSize,
            vk::BufferUsageFlagBits::eIndexBuffer |
                vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::
                    eAccelerationStructureBuildInputReadOnlyKHR |
                vk::BufferUsageFlagBits::eStorageBuffer,
            &m_indexBuffer, indices.data()))
        return false;

    m_device->setDebugObjectName(*m_indexBuffer.buffer, "IndexBuffer");
    m_indexBuffer.descInfo =
        vk::DescriptorBufferInfo(*m_indexBuffer.buffer, 0, indicesSize);

    return true;
}

std::unique_ptr<kirana::viewport::vulkan::Pipeline> kirana::viewport::vulkan::
    SceneData::getPipelineForMaterial(const scene::Material &material)
{
    // TODO: Add support for raytrace materials / pipeline.
    const scene::Material::MaterialProperties &prop = material.getProperties();
    const RasterPipeline::Properties properties = {
        vk::PrimitiveTopology::eTriangleList,
        prop.renderWireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill,
        vk::CullModeFlags(static_cast<VkCullModeFlagBits>(prop.cullMode)),
        prop.wireframeWidth,
        vk::SampleCountFlagBits::e1,
        prop.surfaceType == scene::Material::SurfaceType::TRANSPARENT,
        prop.enableDepth,
        prop.writeDepth,
        static_cast<vk::CompareOp>(prop.depthCompareOp),
        prop.stencil.enableTest,
        static_cast<vk::CompareOp>(prop.stencil.compareOp),
        static_cast<vk::StencilOp>(prop.stencil.failOp),
        static_cast<vk::StencilOp>(prop.stencil.depthFailOp),
        static_cast<vk::StencilOp>(prop.stencil.passOp),
        prop.stencil.reference};

    const std::vector<const DescriptorSetLayout *> descLayouts{
        m_globalDescSetLayout, m_objectDescSetLayout};

    return std::make_unique<RasterPipeline>(
        m_device, m_renderPass, descLayouts,
        std::vector<const PushConstantBase *>(), material.getName(),
        material.getShader(), m_vertexDesc, properties);
}

void kirana::viewport::vulkan::SceneData::createMaterials()
{
    for (const auto &m : m_scene.getSceneMaterials())
        m_materials[m->getName()] = std::move(getPipelineForMaterial(*m));
}

const std::unique_ptr<kirana::viewport::vulkan::Pipeline>
    &kirana::viewport::vulkan::SceneData::findMaterial(
        const std::string &materialName, bool overrideShading)
{
    if (overrideShading)
        return m_materials[materialName];

    switch (m_currentShading)
    {
    case viewport::ShadingPipeline::BASIC:
        return m_materials[scene::Material::DEFAULT_MATERIAL_BASIC_SHADED
                               .getName()];
    case viewport::ShadingPipeline::WIREFRAME:
        return m_materials[scene::Material::DEFAULT_MATERIAL_WIREFRAME
                               .getName()];
    default:
    case viewport::ShadingPipeline::REALTIME_PBR:
        // TODO: Add PBR Pipeline
    case viewport::ShadingPipeline::RAYTRACE_PBR:
        // TODO: Add Raytrace PBR Pipeline
        return m_materials[materialName];
    }
}


bool kirana::viewport::vulkan::SceneData::hasMeshData(
    const std::string &meshName, uint32_t *meshIndex) const
{
    if (m_meshes.empty())
        return false;

    const auto it = std::find_if(
        m_meshes.begin(), m_meshes.end(),
        [meshName](const MeshData &m) { return m.name == meshName; });

    if (it != m_meshes.end())
    {
        *meshIndex = it->index;
        return true;
    }
    return false;
}

bool kirana::viewport::vulkan::SceneData::createMeshes()
{
    m_totalInstanceCount = 0;
    uint32_t meshIndex = 0;
    uint32_t instanceIndex = 0;
    std::vector<scene::Vertex> vertices;
    std::vector<uint32_t> indices;
    m_meshes.clear();
    for (const auto &r : m_scene.getSceneRenderables())
    {
        InstanceData instance{instanceIndex++, r.object->transform, &r.selected,
                              &r.renderVisible};
        for (const auto &m : r.object->getMeshes())
        {
            const std::string &meshName = m->getName();
            uint32_t index = meshIndex;
            // If there is already a mesh, we just store the instance.
            if (hasMeshData(meshName, &index))
                m_meshes[index].instances.emplace_back(instance);
            else
            {
                MeshData meshData{};
                meshData.index = meshIndex++;
                meshData.name = m->getName();
                meshData.render = false; // Make it non-renderable at first

                const auto &meshVertices = m->getVertices();
                const auto &meshIndices = m->getIndices();
                meshData.vertexCount =
                    static_cast<uint32_t>(meshVertices.size());
                meshData.indexCount = static_cast<uint32_t>(meshIndices.size());
                // Get the offset into the global vertex and index buffer.
                meshData.firstIndex = static_cast<uint32_t>(indices.size());
                meshData.vertexOffset = static_cast<uint32_t>(vertices.size());
                // Add the mesh vertices and indices to the global vertex and
                // index buffer.
                vertices.insert(vertices.end(), meshVertices.begin(),
                                meshVertices.end());
                indices.insert(indices.end(), meshIndices.begin(),
                               meshIndices.end());

                instanceIndex = 0;
                instance.index = instanceIndex;
                meshData.instances.emplace_back(instance);

                meshData.material = findMaterial(m->getMaterial()->getName(),
                                                 r.overrideMaterial)
                                        .get();

                m_meshes.emplace_back(std::move(meshData));
            }
            // If any one of the instance is visible, render it.
            if (r.renderVisible)
                m_meshes[index].render = true;

            ++m_totalInstanceCount;
        }
    }
    return createVertexAndIndexBuffer(vertices, indices);
}

bool kirana::viewport::vulkan::SceneData::createObjectBuffer()
{
    const vk::DeviceSize bufferSize = constants::VULKAN_FRAME_OVERLAP_COUNT *
                                      sizeof(vulkan::ObjectData) *
                                      m_totalInstanceCount;
    if (m_allocator->allocateBuffer(
            bufferSize, vk::BufferUsageFlagBits::eStorageBuffer,
            vma::MemoryUsage::eCpuToGpu, &m_objectBuffer))
    {
        m_objectBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_objectBuffer.buffer, 0,
            sizeof(vulkan::ObjectData) * m_totalInstanceCount);

        m_device->setDebugObjectName(*m_objectBuffer.buffer, "ObjectBuffer");
        onObjectChanged();
        return true;
    }
    return false;
}


bool kirana::viewport::vulkan::SceneData::initializeRaytracing()
{
    if (m_vertexBuffer.buffer == nullptr || m_indexBuffer.buffer == nullptr)
        return false;

    // TODO: Separate the following into functions.
    const vk::DeviceAddress &vertexBufferAddress =
        m_device->getBufferAddress(*m_vertexBuffer.buffer);
    const vk::DeviceAddress &indexBufferAddress =
        m_device->getBufferAddress(*m_indexBuffer.buffer);

    m_raytraceGlobalData = new PushConstant<PushConstantRaytrace>(
        {vertexBufferAddress, indexBufferAddress, 0, 10, 8},
        vk::ShaderStageFlagBits::eRaygenKHR |
            vk::ShaderStageFlagBits::eClosestHitKHR);

    std::vector<RaytracedObjectData> objectData;
    for (const auto &m : m_meshes)
        for (const auto &i : m.instances)
            objectData.emplace_back(
                RaytracedObjectData{m.firstIndex, m.vertexOffset});

    const vk::DeviceSize objDataSize =
        sizeof(RaytracedObjectData) * objectData.size();
    if (!m_allocator->allocateBufferToGPU(
            objDataSize,
            vk::BufferUsageFlagBits::eStorageBuffer |
                vk::BufferUsageFlagBits::eShaderDeviceAddress,
            &m_raytracedObjectBuffer,
            reinterpret_cast<void *>(objectData.data())))
        return false;

    m_raytracedObjectBuffer.descInfo = vk::DescriptorBufferInfo(
        *m_raytracedObjectBuffer.buffer, 0, objDataSize);
    m_device->setDebugObjectName(*m_raytracedObjectBuffer.buffer,
                                 "Raytraced_ObjectBuffer");

    m_accelStructure =
        new AccelerationStructure(m_device, m_allocator, m_meshes,
                                  vertexBufferAddress, indexBufferAddress);

    if (m_accelStructure->isInitialized)
    {
        const std::vector<const DescriptorSetLayout *> descLayouts{
            m_globalDescSetLayout, m_raytraceDescSetLayout};
        const std::vector<const PushConstantBase *> pushConstants{
            m_raytraceGlobalData};
        m_raytracePipeline = new RaytracePipeline(
            m_device, m_renderPass, descLayouts, pushConstants,
            constants::DEFAULT_MATERIAL_RAYTRACE_NAME,
            constants::VULKAN_SHADER_RAYTRACE_NAME);
    }
    if (m_raytracePipeline && m_raytracePipeline->isInitialized)
        m_shaderBindingTable =
            new ShaderBindingTable(m_device, m_allocator, m_raytracePipeline);

    return m_shaderBindingTable != nullptr &&
           m_shaderBindingTable->isInitialized;
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const RenderPass *renderPass, const scene::ViewportScene &scene,
    viewport::ShadingPipeline shading)
    : m_isInitialized{false}, m_currentShading{shading},
      m_totalInstanceCount{0}, m_device{device}, m_allocator{allocator},
      m_renderPass{renderPass}, m_globalDescSetLayout{new DescriptorSetLayout(
                                    m_device)},
      m_objectDescSetLayout{new DescriptorSetLayout(
          m_device, DescriptorSetLayout::LayoutType::OBJECT)},
      m_raytraceDescSetLayout{new DescriptorSetLayout(
          m_device, DescriptorSetLayout::LayoutType::RAYTRACE)},
      m_scene{scene}
{
    // Set the vulkan description of vertex buffers.
    setVertexDescription();

    // Create Camera data buffer.
    createCameraBuffer();
    // Create World data buffer.
    createWorldDataBuffer();

    // Add listeners
    m_worldChangeListener = m_scene.addOnWorldChangeEventListener(
        [&]() { this->onWorldChanged(); });
    m_cameraChangeListener = m_scene.addOnCameraChangeEventListener(
        [&]() { this->onCameraChanged(); });
    m_sceneLoadListener = m_scene.addOnSceneLoadedEventListener(
        [&](bool result) { this->onSceneLoaded(result); });

    // Create shaders, pipeline layouts and pipelines for
    // all the materials in the scene.
    m_materials.clear();
    createMaterials();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_meshes.clear();
    m_isInitialized = createMeshes();
    m_isInitialized = createObjectBuffer();
    m_isInitialized = initializeRaytracing();
    if (m_isInitialized)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Generated scene data for viewport");
    }
    else
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            "Failed to allocate buffer to some of the scene objects");
    }
}

kirana::viewport::vulkan::SceneData::~SceneData()
{
    m_scene.removeOnSceneLoadedEventListener(m_sceneLoadListener);
    m_scene.removeOnWorldChangeEventListener(m_worldChangeListener);
    m_scene.removeOnCameraChangeEventListener(m_cameraChangeListener);

    if (m_raytracedObjectBuffer.buffer)
    {
        m_allocator->free(m_raytracedObjectBuffer);
    }
    if (m_shaderBindingTable)
    {
        delete m_shaderBindingTable;
        m_shaderBindingTable = nullptr;
    }

    if (m_raytracePipeline)
    {
        delete m_raytracePipeline;
        m_raytracePipeline = nullptr;
    }

    if (m_accelStructure)
    {
        delete m_accelStructure;
        m_accelStructure = nullptr;
    }

    if (m_raytraceGlobalData)
    {
        delete m_raytraceGlobalData;
        m_raytraceGlobalData = nullptr;
    }

    if (m_objectBuffer.buffer)
    {
        m_allocator->free(m_objectBuffer);
    }
    if (m_vertexBuffer.buffer)
    {
        m_allocator->free(m_vertexBuffer);
    }
    if (m_indexBuffer.buffer)
    {
        m_allocator->free(m_indexBuffer);
    }
    if (m_worldDataBuffer.buffer)
    {
        m_allocator->free(m_worldDataBuffer);
    }
    if (m_cameraBuffer.buffer)
    {
        m_allocator->free(m_cameraBuffer);
    }
    if (m_raytraceDescSetLayout)
    {
        delete m_raytraceDescSetLayout;
        m_raytraceDescSetLayout = nullptr;
    }
    if (m_objectDescSetLayout)
    {
        delete m_objectDescSetLayout;
        m_objectDescSetLayout = nullptr;
    }
    if (m_globalDescSetLayout)
    {
        delete m_globalDescSetLayout;
        m_globalDescSetLayout = nullptr;
    }
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Scene data destroyed");
}

void kirana::viewport::vulkan::SceneData::updateRaytracedFrameCount(bool reset)
{
    if (reset || m_currentShading != ShadingPipeline::RAYTRACE_PBR)
        m_raytracedFrameCount = 0;

    if (m_raytraceGlobalData == nullptr)
        return;
    auto data = m_raytraceGlobalData->get();
    data.frameIndex = m_raytracedFrameCount;
    m_raytraceGlobalData->set(data);

    m_raytracedFrameCount++;
}

[[nodiscard]] const kirana::viewport::vulkan::PushConstant<
    kirana::viewport::vulkan::PushConstantRaytrace>
    &kirana::viewport::vulkan::SceneData::getRaytracedGlobalData() const
{
    return *m_raytraceGlobalData;
}

const kirana::viewport::vulkan::Pipeline *kirana::viewport::vulkan::SceneData::
    getOutlineMaterial() const
{
    return m_materials[scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE
                           .getName()]
        .get();
}

const kirana::scene::WorldData &kirana::viewport::vulkan::SceneData::
    getWorldData() const
{
    return m_scene.getWorldData();
}

uint32_t kirana::viewport::vulkan::SceneData::getCameraBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(m_device->alignUniformBufferSize(
        sizeof(scene::CameraData) * offsetIndex));
}

uint32_t kirana::viewport::vulkan::SceneData::getWorldDataBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(m_device->alignUniformBufferSize(
        sizeof(scene::WorldData) * offsetIndex));
}

uint32_t kirana::viewport::vulkan::SceneData::getObjectBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(sizeof(vulkan::ObjectData) *
                                 m_totalInstanceCount * offsetIndex);
}

const vk::AccelerationStructureKHR &kirana::viewport::vulkan::SceneData::
    getAccelerationStructure() const
{
    return m_accelStructure->getAccelerationStructure();
}