#include "scene_data.hpp"
#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"
#include "descriptor_set.hpp"
#include "raytrace_data.hpp"
#include "material_manager.hpp"
#include "pipeline_layout.hpp"
#include "push_constant.hpp"

#include "vulkan_utils.hpp"

#include <algorithm>
#include <viewport_scene.hpp>

void kirana::viewport::vulkan::SceneData::onWorldChanged()
{
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::WorldData));

    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToBuffer(m_worldDataBuffer,
                                      &m_scene.getWorldData(), paddedSize * i,
                                      sizeof(scene::WorldData));
    }
}

void kirana::viewport::vulkan::SceneData::onCameraChanged()
{
    const auto &cameraData = m_scene.getCameraData();
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::CameraData));
    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToBuffer(m_cameraBuffer, &cameraData,
                                      paddedSize * i,
                                      sizeof(scene::CameraData));
    }
}

void kirana::viewport::vulkan::SceneData::onSceneLoaded(bool result)
{
    if (result)
    {
        createSceneMaterials();
        createMeshes(false);
        createObjectBuffer();
    }
}


void kirana::viewport::vulkan::SceneData::onObjectChanged()
{
    std::vector<vulkan::ObjectData> objData;
    for (const auto &m : m_sceneMeshes)
    {
        for (const auto &i : m.instances)
        {
            objData.emplace_back(ObjectData{
                getVertexBufferAddress(m.vertexBufferIndex),
                getIndexBufferAddress(m.indexBufferIndex),
                m_materialManager->getMaterialDataBufferAddress(
                    m.materialIndex),
                m_materialManager->getMaterialDataIndex(m.materialIndex),
                m.firstIndex, m.vertexOffset});
        }
    }
    m_allocator->copyDataToBuffer(m_objectDataBuffer, objData.data(), 0,
                                  sizeof(vulkan::ObjectData) * objData.size());
}

void kirana::viewport::vulkan::SceneData::createWorldDataBuffer()
{
    // TODO: Make World buffer device local and host-visible if supported
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::WorldData));
    const vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(&m_worldDataBuffer, bufferSize,
                                    vk::BufferUsageFlagBits::eUniformBuffer,
                                    Allocator::AllocationType::WRITEABLE))
    {
        m_worldDataBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_worldDataBuffer.buffer, 0, sizeof(scene::WorldData));

        m_device->setDebugObjectName(*m_cameraBuffer.buffer, "WorldBuffer");

        const auto &bindingInfo = DescriptorSetLayout::getBindingInfoForData(
            DescriptorBindingDataType::WORLD, ShadingPipeline::RASTER);
        m_rasterDescSets[static_cast<int>(bindingInfo.layoutType)].bindBuffer(
            bindingInfo, m_worldDataBuffer);

        onWorldChanged();
    }
}

void kirana::viewport::vulkan::SceneData::createCameraBuffer()
{
    // TODO: Make Camera buffer device local and host-visible if supported
    const vk::DeviceSize paddedSize =
        m_device->alignUniformBufferSize(sizeof(scene::CameraData));
    const vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(&m_cameraBuffer, bufferSize,
                                    vk::BufferUsageFlagBits::eUniformBuffer,
                                    Allocator::AllocationType::WRITEABLE))
    {
        m_cameraBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_cameraBuffer.buffer, 0, sizeof(scene::CameraData));

        m_device->setDebugObjectName(*m_cameraBuffer.buffer, "CameraBuffer");

        const auto &bindingInfo = DescriptorSetLayout::getBindingInfoForData(
            DescriptorBindingDataType::CAMERA, ShadingPipeline::RASTER);
        m_rasterDescSets[static_cast<int>(bindingInfo.layoutType)].bindBuffer(
            bindingInfo, m_cameraBuffer);

        onCameraChanged();
    }
}

std::pair<int, int> kirana::viewport::vulkan::SceneData::
    createVertexAndIndexBuffer(const std::vector<scene::Vertex> &vertices,
                               const std::vector<scene::INDEX_TYPE> &indices)
{
    const size_t totalVertexSize = vertices.size() * sizeof(scene::Vertex);
    if (m_vertexBuffers.empty() ||
        ((totalVertexSize + m_vertexBuffers.back().currentSize) >
         constants::VULKAN_VERTEX_BUFFER_BATCH_SIZE_LIMIT))
    {
        const size_t bufferSize = std::max(
            totalVertexSize, constants::VULKAN_VERTEX_BUFFER_BATCH_SIZE_LIMIT);

        BatchBufferData buffer{};

        if (!m_allocator->allocateBuffer(
                &buffer.buffer, bufferSize,
                vk::BufferUsageFlagBits::eVertexBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress |
                    vk::BufferUsageFlagBits::eStorageBuffer |
                    RaytraceData::VERTEX_INDEX_BUFFER_USAGE_FLAGS,
                Allocator::AllocationType::GPU_WRITEABLE))
            return {-1, -1};

        m_device->setDebugObjectName(
            *buffer.buffer.buffer,
            "VertexBuffer_" + std::to_string(m_vertexBuffers.size()));
        buffer.currentSize = 0;
        buffer.currentDataCount = 0;
        m_vertexBuffers.emplace_back(std::move(buffer));
    }
    auto &vBuffer = m_vertexBuffers.back();


    m_allocator->copyDataToBuffer(vBuffer.buffer, vertices.data(),
                                  vBuffer.currentSize, totalVertexSize);
    vBuffer.currentSize += totalVertexSize;
    vBuffer.currentDataCount += vertices.size();


    const size_t totalIndexSize = indices.size() * sizeof(scene::INDEX_TYPE);

    if (m_indexBuffers.empty() ||
        ((totalIndexSize + m_indexBuffers.back().currentSize) >
         constants::VULKAN_INDEX_BUFFER_BATCH_SIZE_LIMIT))
    {
        const size_t bufferSize = std::max(
            totalIndexSize, constants::VULKAN_INDEX_BUFFER_BATCH_SIZE_LIMIT);

        BatchBufferData buffer{};
        if (!m_allocator->allocateBuffer(
                &buffer.buffer, bufferSize,
                vk::BufferUsageFlagBits::eIndexBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress |
                    vk::BufferUsageFlagBits::eStorageBuffer |
                    RaytraceData::VERTEX_INDEX_BUFFER_USAGE_FLAGS,
                Allocator::AllocationType::GPU_WRITEABLE))
            return {-1, -1};

        m_device->setDebugObjectName(*buffer.buffer.buffer,
                                     "IndexBuffer_" +
                                         std::to_string(m_indexBuffers.size()));

        buffer.currentSize = 0;
        buffer.currentDataCount = 0;
        m_indexBuffers.emplace_back(std::move(buffer));
    }
    auto &iBuffer = m_indexBuffers.back();

    m_allocator->copyDataToBuffer(iBuffer.buffer, indices.data(),
                                  iBuffer.currentSize, totalIndexSize);
    iBuffer.currentSize += totalIndexSize;
    iBuffer.currentDataCount += indices.size();

    return {static_cast<int>(m_vertexBuffers.size() - 1),
            static_cast<int>(m_indexBuffers.size() - 1)};
}

void kirana::viewport::vulkan::SceneData::createEditorMaterials()
{
    for (const auto &em : m_scene.getEditorMaterials())
        m_materialManager->addMaterial(*m_renderPass, *em);
}

void kirana::viewport::vulkan::SceneData::createSceneMaterials()
{
    for (const auto &m : m_scene.getSceneMaterials())
        m_materialManager->addMaterial(*m_renderPass, *m);
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

                auto bufferIndices =
                    createVertexAndIndexBuffer(meshVertices, meshIndices);
                if (bufferIndices.first == -1 || bufferIndices.second == -1)
                {
                    Logger::get().log(
                        constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                        "Failed to put data in vertex/index buffer for mesh: " +
                            m->getName());
                }
                meshData.vertexBufferIndex = bufferIndices.first;
                meshData.indexBufferIndex = bufferIndices.second;

                // Get the offset into the global vertex and index buffer.
                meshData.vertexOffset = static_cast<uint32_t>(
                    m_vertexBuffers[bufferIndices.first].currentDataCount -
                    meshData.vertexCount);
                meshData.firstIndex = static_cast<uint32_t>(
                    m_indexBuffers[bufferIndices.second].currentDataCount -
                    meshData.indexCount);

                int matIndex = m_materialManager->getMaterialIndexFromName(
                    m->getMaterial()->getName());
                if (matIndex == -1)
                {
                    matIndex = m_materialManager->getMaterialIndexFromName(
                        scene::Material::DEFAULT_MATERIAL_BASIC_SHADED
                            .getName());
                }
                meshData.materialIndex = static_cast<uint32_t>(matIndex);

                instanceIndex = 0;
                instance.index = instanceIndex;
                meshData.instances.emplace_back(instance);

                meshes.emplace_back(std::move(meshData));
            }
            // If any one of the instance is visible, render it.
            if (r.renderVisible)
                meshes[index].render = true;
        }
    }
    return true;
}

void kirana::viewport::vulkan::SceneData::createObjectBuffer()
{
    // TODO: Make Object Data buffer device local.
    const vk::DeviceSize bufferSize =
        sizeof(vulkan::ObjectData) * m_scene.getSceneInfo().numObjects;
    if (m_allocator->allocateBuffer(&m_objectDataBuffer, bufferSize,
                                    vk::BufferUsageFlagBits::eStorageBuffer,
                                    Allocator::AllocationType::WRITEABLE))
    {
        m_objectDataBuffer.descInfo =
            vk::DescriptorBufferInfo(*m_objectDataBuffer.buffer, 0, bufferSize);

        m_device->setDebugObjectName(*m_objectDataBuffer.buffer,
                                     "ObjectDataBuffer");
        onObjectChanged();
    }
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const DescriptorPool *const descriptorPool, const RenderPass *renderPass,
    RaytraceData *raytraceData, const scene::ViewportScene &scene,
    vulkan::ShadingPipeline pipeline, vulkan::ShadingType type)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_descriptorPool{descriptorPool}, m_renderPass{renderPass},
      m_raytraceData{raytraceData}, m_scene{scene}, m_raytracedFrameCount{0},
      m_materialManager{
          new MaterialManager(m_device, m_allocator, m_descriptorPool)}
{
    m_isInitialized = PipelineLayout::getDefaultPipelineLayout(
        m_device, ShadingPipeline::RASTER, m_rasterPipelineLayout);
    if (m_isInitialized)
    {
        const auto &descLayouts =
            m_rasterPipelineLayout->getDescriptorSetLayouts();
        m_rasterDescSets.resize(descLayouts.size());
        m_descriptorPool->allocateDescriptorSets(descLayouts,
                                                 &m_rasterDescSets);
    }

    // Create Camera data buffer.
    createCameraBuffer();
    // Create World data buffer.
    createWorldDataBuffer();

    for (const auto &s : m_rasterDescSets)
        m_descriptorPool->writeDescriptorSet(s);

    // Add listeners
    m_worldChangeListener = m_scene.addOnWorldChangeEventListener(
        [&]() { this->onWorldChanged(); });
    m_cameraChangeListener = m_scene.addOnCameraChangeEventListener(
        [&]() { this->onCameraChanged(); });

    createEditorMaterials();
    m_isInitialized = createMeshes(true);
    if (m_scene.isSceneLoaded())
        onSceneLoaded(true);

    m_sceneLoadListener = m_scene.addOnSceneLoadedEventListener(
        [&](bool result) { this->onSceneLoaded(result); });


    setShadingPipeline(pipeline);
    setShadingType(type);

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

    for (auto &v : m_vertexBuffers)
    {
        if (v.buffer.buffer)
            m_allocator->free(v.buffer);
    }
    for (auto &i : m_indexBuffers)
    {
        if (i.buffer.buffer)
            m_allocator->free(i.buffer);
    }
    if (m_objectDataBuffer.buffer)
    {
        m_allocator->free(m_objectDataBuffer);
    }
    if (m_worldDataBuffer.buffer)
    {
        m_allocator->free(m_worldDataBuffer);
    }
    if (m_cameraBuffer.buffer)
    {
        m_allocator->free(m_cameraBuffer);
    }
    if (m_rasterPipelineLayout)
    {
        delete m_rasterPipelineLayout;
        m_rasterPipelineLayout = nullptr;
    }
    if (m_materialManager)
    {
        delete m_materialManager;
        m_materialManager = nullptr;
    }
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Scene data destroyed");
}

void kirana::viewport::vulkan::SceneData::setShadingPipeline(
    vulkan::ShadingPipeline pipeline)
{
    m_currentShadingPipeline = pipeline;
    if (m_currentShadingPipeline == ShadingPipeline::RAYTRACE &&
        !m_raytraceData->isInitialized)
        m_raytraceData->initialize(*this);
}

void kirana::viewport::vulkan::SceneData::setShadingType(
    vulkan::ShadingType type)
{
    m_currentShadingType = type;
}

const kirana::viewport::vulkan::Pipeline &kirana::viewport::vulkan::SceneData::
    getCurrentPipeline(bool isEditorMesh, uint32_t meshIndex) const
{
    if (isEditorMesh)
        return *m_materialManager->getPipeline(
            m_editorMeshes[meshIndex].materialIndex, m_currentShadingPipeline);

    switch (m_currentShadingType)
    {
    case ShadingType::PBR:
        return *m_materialManager->getPipeline(
            m_sceneMeshes[meshIndex].materialIndex, m_currentShadingPipeline);
    case ShadingType::WIREFRAME:
        return *m_materialManager->getPipeline(
            m_materialManager->getMaterialIndexFromName(
                scene::Material::DEFAULT_MATERIAL_WIREFRAME.getName()),
            m_currentShadingPipeline);
    default:
    case ShadingType::BASIC:
        return *m_materialManager->getPipeline(
            m_materialManager->getMaterialIndexFromName(
                scene::Material::DEFAULT_MATERIAL_BASIC_SHADED.getName()),
            m_currentShadingPipeline);
    }
}

const kirana::viewport::vulkan::Pipeline *kirana::viewport::vulkan::SceneData::
    getOutlineMaterial() const
{
    return m_materialManager->getPipeline(
        m_materialManager->getMaterialIndexFromName(
            scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE.getName()),
        m_currentShadingPipeline);
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

kirana::viewport::vulkan::PushConstant<
    kirana::viewport::vulkan::PushConstantRaster>
kirana::viewport::vulkan::SceneData::getPushConstantRasterData(
    bool isEditor, uint32_t meshIndex, uint32_t instanceIndex) const
{
    const MeshData &meshData =
        isEditor ? m_editorMeshes[meshIndex] : m_sceneMeshes[meshIndex];

    return PushConstant<PushConstantRaster>(
        {meshData.instances[instanceIndex].transform->getMatrix(),
         getVertexBufferAddress(meshData.vertexBufferIndex),
         getIndexBufferAddress(meshData.indexBufferIndex),
         m_materialManager->getMaterialDataBufferAddress(
             meshData.materialIndex),
         m_materialManager->getMaterialDataIndex(meshData.materialIndex),
         meshData.getGlobalInstanceIndex(instanceIndex), meshData.firstIndex,
         meshData.vertexOffset},
        vulkan::PUSH_CONSTANT_RASTER_SHADER_STAGES);
}

kirana::viewport::vulkan::PushConstant<
    kirana::viewport::vulkan::PushConstantRaytrace>
kirana::viewport::vulkan::SceneData::getPushConstantRaytraceData() const
{
    return PushConstant<PushConstantRaytrace>(
        {}, vulkan::PUSH_CONSTANT_RAYTRACE_SHADER_STAGES);
}