#include "scene_data.hpp"
#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "allocator.hpp"
#include "shader.hpp"
#include "descriptor_set_layout.hpp"
#include "pipeline_layout.hpp"
#include "raster_pipeline.hpp"
#include "acceleration_structure.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "push_constant.hpp"
#include "material_manager.hpp"
#include "raytrace_data.hpp"

#include <algorithm>
#include <viewport_scene.hpp>

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
        for (const auto &m : m_sceneMeshes)
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

bool kirana::viewport::vulkan::SceneData::transferBufferToGPU(
    BatchBufferData &bufferData)
{
    if (bufferData.cpuBuffer.buffer != nullptr &&
        bufferData.gpuBuffer.buffer == nullptr)
    {
        if (m_allocator->allocateBuffer(
                bufferData.currentSize,
                vk::BufferUsageFlagBits::eStorageBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress |
                    vk::BufferUsageFlagBits::eTransferDst |
                    RaytraceData::VERTEX_INDEX_BUFFER_USAGE_FLAGS,
                vma::MemoryUsage::eGpuOnly, &bufferData.gpuBuffer, false))
        {
            if (!m_allocator->copyBuffer(bufferData.cpuBuffer,
                                         bufferData.gpuBuffer,
                                         bufferData.currentSize))
            {
                Logger::get().log(
                    constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                    "Failed to transfer data from CPU to GPU buffer");
                return false;
            }
            m_allocator->free(bufferData.cpuBuffer);
            return true;
        }
        else
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to create GPU buffer");
            return false;
        }
    }
    return true;
}

std::pair<int, int> kirana::viewport::vulkan::SceneData::
    createVertexAndIndexBuffer(const std::vector<scene::Vertex> &vertices,
                               const std::vector<scene::INDEX_TYPE> &indices)
{
    const size_t totalVertexSize = vertices.size() * sizeof(scene::Vertex);

    if (m_vertexBuffers.empty() ||
        (totalVertexSize + m_vertexBuffers.back().currentSize) >
            constants::VULKAN_VERTEX_BUFFER_BATCH_SIZE_LIMIT)
    {
        // The previous buffer is filled, move it to the GPU.
        if (!m_vertexBuffers.empty())
            transferBufferToGPU(m_vertexBuffers.back());
        // Create a new vertex buffer
        BatchBufferData batch{};

        if (!m_allocator->allocateBuffer(
                constants::VULKAN_VERTEX_BUFFER_BATCH_SIZE_LIMIT,
                vk::BufferUsageFlagBits::eTransferSrc,
                vma::MemoryUsage::eCpuToGpu, &batch.cpuBuffer, true))
            return {-1, -1};

        batch.currentMemoryPointer =
            reinterpret_cast<char *>(batch.cpuBuffer.memoryPointer);
        m_vertexBuffers.emplace_back(std::move(batch));
    }

    memcpy(
        reinterpret_cast<void *>(m_vertexBuffers.back().currentMemoryPointer),
        reinterpret_cast<const void *>(vertices.data()), totalVertexSize);
    m_vertexBuffers.back().currentMemoryPointer += totalVertexSize;
    m_vertexBuffers.back().currentSize += totalVertexSize;
    m_vertexBuffers.back().currentDataOffset += vertices.size();


    const size_t totalIndexSize = indices.size() * sizeof(scene::INDEX_TYPE);

    if (m_indexBuffers.empty() ||
        (totalIndexSize + m_indexBuffers.back().currentSize) >
            constants::VULKAN_INDEX_BUFFER_BATCH_SIZE_LIMIT)
    {
        // The previous buffer is filled, move it to the GPU.
        if (!m_indexBuffers.empty())
            transferBufferToGPU(m_indexBuffers.back());
        // Create a new vertex buffer
        BatchBufferData batch{};
        if (!m_allocator->allocateBuffer(
                constants::VULKAN_INDEX_BUFFER_BATCH_SIZE_LIMIT,
                vk::BufferUsageFlagBits::eTransferSrc,
                vma::MemoryUsage::eCpuToGpu, &batch.cpuBuffer, true))
            return {-1, -1};
        batch.currentMemoryPointer =
            reinterpret_cast<char *>(batch.cpuBuffer.memoryPointer);
        m_indexBuffers.emplace_back(std::move(batch));
    }

    memcpy(reinterpret_cast<void *>(m_indexBuffers.back().currentMemoryPointer),
           reinterpret_cast<const void *>(indices.data()), totalIndexSize);
    m_indexBuffers.back().currentMemoryPointer += totalIndexSize;
    m_indexBuffers.back().currentSize += totalIndexSize;
    m_indexBuffers.back().currentDataOffset += indices.size();

    return {static_cast<int>(m_vertexBuffers.size() - 1),
            static_cast<int>(m_indexBuffers.size() - 1)};
}

void kirana::viewport::vulkan::SceneData::createEditorMaterials()
{
    for (const auto &em : m_scene.getEditorMaterials())
    {
        const int index = m_materialManager->addMaterial(*m_renderPass, *em);
        if (index == -1)
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to create material: " + em->getName());
        }
    }
}

void kirana::viewport::vulkan::SceneData::createSceneMaterials()
{
    for (const auto &m : m_scene.getSceneMaterials())
    {
        const int index = m_materialManager->addMaterial(*m_renderPass, *m);
        if (index == -1)
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to create material: " + m->getName());
        }
    }
}


bool kirana::viewport::vulkan::SceneData::hasMeshData(
    const std::vector<MeshData> &meshes, const std::string &meshName,
    uint32_t *meshIndex)
{
    if (meshes.empty())
        return false;

    const auto it = std::find_if(
        meshes.begin(), meshes.end(),
        [meshName](const MeshData &m) { return m.name == meshName; });

    if (it != meshes.end())
    {
        *meshIndex = it->index;
        return true;
    }
    return false;
}

bool kirana::viewport::vulkan::SceneData::createMeshes(bool isEditor)
{
    uint32_t meshIndex = 0;
    uint32_t instanceIndex = 0;

    std::vector<MeshData> &meshes = isEditor ? m_editorMeshes : m_sceneMeshes;

    const std::vector<scene::Renderable> &renderables =
        isEditor ? m_scene.getEditorRenderables()
                 : m_scene.getSceneRenderables();
    for (const auto &r : renderables)
    {
        InstanceData instance{instanceIndex++, r.object->transform,
                              &r.viewportVisible, &r.renderVisible,
                              &r.selected};
        for (const auto &m : r.object->getMeshes())
        {
            const std::string &meshName = m->getName();
            uint32_t index = meshIndex;
            // If there is already a mesh, we just store the instance.
            if (hasMeshData(meshes, meshName, &index))
                meshes[index].instances.emplace_back(instance);
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

                auto indices =
                    createVertexAndIndexBuffer(meshVertices, meshIndices);
                if (indices.first == -1 || indices.second == -1)
                {
                    Logger::get().log(
                        constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                        "Failed to put data in vertex/index buffer for mesh: " +
                            m->getName());
                }
                meshData.vertexBufferIndex = indices.first;
                meshData.indexBufferIndex = indices.second;

                // Get the offset into the global vertex and index buffer.
                meshData.firstIndex = static_cast<uint32_t>(
                    m_indexBuffers[indices.second].currentDataOffset);
                meshData.vertexOffset = static_cast<uint32_t>(
                    m_vertexBuffers[indices.first].currentDataOffset);

                instanceIndex = 0;
                instance.index = instanceIndex;
                meshData.instances.emplace_back(instance);

                int matIndex = m_materialManager->getMaterialIndexFromName(
                    m->getMaterial()->getName());
                if (matIndex == -1)
                {
                    matIndex = m_materialManager->getMaterialIndexFromName(
                        scene::Material::DEFAULT_MATERIAL_BASIC_SHADED
                            .getName());
                }
                meshData.materialIndex = static_cast<uint32_t>(matIndex);
                meshes.emplace_back(std::move(meshData));
            }
            // If any one of the instance is visible, render it.
            if (r.renderVisible)
                meshes[index].render = true;

            ++m_totalInstanceCount;
        }
    }
    transferBufferToGPU(m_vertexBuffers.back());
    transferBufferToGPU(m_indexBuffers.back());
    return true;
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

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const RenderPass *renderPass, const RaytraceData *raytraceData,
    const scene::ViewportScene &scene, vulkan::ShadingPipeline pipeline,
    vulkan::ShadingType type)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_renderPass{renderPass}, m_raytraceData{raytraceData}, m_scene{scene},
      m_currentShadingPipeline{pipeline}, m_currentShadingType{type},
      m_raytracedFrameCount{0}, m_totalInstanceCount{0},
      m_materialManager{new MaterialManager(m_device, m_allocator)}
{
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

    createEditorMaterials();
    createMeshes(true);
    if (m_scene.isSceneLoaded())
    {
        createSceneMaterials();
        createMeshes(false);
    }

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
    if (m_materialManager)
    {
        delete m_materialManager;
        m_materialManager = nullptr;
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