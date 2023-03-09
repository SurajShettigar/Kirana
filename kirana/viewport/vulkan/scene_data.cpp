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
    m_onSceneDataChange();
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
    m_onSceneDataChange();
}

void kirana::viewport::vulkan::SceneData::onSceneLoaded(bool result)
{
    if (result)
    {
        createMaterials(false);
        createMeshes(false);
        createObjectBuffer();
    }
    m_onSceneDataChange();
}


void kirana::viewport::vulkan::SceneData::onObjectChanged()
{
    std::vector<vulkan::ObjectData> objData;
    for (const auto &mObj : m_sceneMeshes)
    {
        for (const auto &m : mObj.meshes)
        {
            const uint32_t matIndex =
                getCurrentMaterialIndex(false, false, mObj.index, m.index);
            objData.emplace_back(ObjectData{
                getVertexBufferAddress(m.vertexBufferIndex),
                getIndexBufferAddress(m.indexBufferIndex),
                m_materialManager->getMaterialDataBufferAddress(matIndex),
                m_materialManager->getMaterialDataIndex(matIndex), m.firstIndex,
                m.vertexOffset});
        }
    }
    if (!objData.empty())
        m_allocator->copyDataToBuffer(m_objectDataBuffer, objData.data(), 0,
                                      sizeof(vulkan::ObjectData) *
                                          objData.size());
    m_onSceneDataChange();
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

        m_device->setDebugObjectName(*m_worldDataBuffer.buffer, "WorldBuffer");

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

void kirana::viewport::vulkan::SceneData::createMaterials(bool isEditor)
{
    const auto &mats =
        isEditor ? m_scene.getEditorMaterials() : m_scene.getSceneMaterials();
    for (const auto &em : mats)
        m_materialManager->addMaterial(*m_renderPass, *em);

    const auto &bindingInfo = DescriptorSetLayout::getBindingInfoForData(
        DescriptorBindingDataType::TEXTURE_DATA, ShadingPipeline::RASTER);
    m_rasterDescSets[static_cast<int>(bindingInfo.layoutType)].bindTextures(
        bindingInfo, m_materialManager->getTextures());
    m_descriptorPool->writeDescriptorSet(
        m_rasterDescSets[static_cast<int>(bindingInfo.layoutType)]);
}


int kirana::viewport::vulkan::SceneData::getMeshObject(
    const std::vector<MeshObjectData> &meshObjects,
    const std::vector<std::shared_ptr<scene::Mesh>> &meshes)
{
    const auto it =
        std::find_if(meshObjects.begin(), meshObjects.end(),
                     [&meshes](const MeshObjectData &mObj) {
                         if (mObj.meshes.size() != meshes.size())
                             return false;
                         for (uint32_t i = 0; i < meshes.size(); i++)
                             if (mObj.meshes[i].name != meshes[i]->getName())
                                 return false;
                         return true;
                     });

    if (it == meshObjects.end())
        return -1;

    return static_cast<int>(it->index);
}

bool kirana::viewport::vulkan::SceneData::createMeshes(bool isEditor)
{
    std::vector<MeshObjectData> &currMeshObjects =
        isEditor ? m_editorMeshes : m_sceneMeshes;

    const std::vector<scene::Renderable> &renderables =
        isEditor ? m_scene.getEditorRenderables()
                 : m_scene.getSceneRenderables();
    uint32_t meshObjIndex = 0;
    for (uint32_t rIndex = 0; rIndex < renderables.size(); rIndex++)
    {
        auto &renderable = renderables[rIndex];
        auto &meshes = renderable.object->getMeshes();

        if (meshes.empty()) // Ignore empty objects
            continue;

        const int foundMeshObjIndex = getMeshObject(currMeshObjects, meshes);
        if (foundMeshObjIndex > -1)
        {
            // If there's already an existing MeshObject with same meshes,
            // create a new instance.
            const uint32_t instanceIndex = static_cast<uint32_t>(
                currMeshObjects[foundMeshObjIndex].instances.size());
            currMeshObjects[foundMeshObjIndex].instances.emplace_back(
                InstanceData{instanceIndex, renderable.object->transform,
                             &renderable.viewportVisible,
                             &renderable.renderVisible, &renderable.selected});
        }
        else
        {
            MeshObjectData meshObject;
            meshObject.index = meshObjIndex++;
            meshObject.name = renderable.object->getName();
            meshObject.instances.emplace_back(InstanceData{
                0, renderable.object->transform, &renderable.viewportVisible,
                &renderable.renderVisible, &renderable.selected});

            for (uint32_t mIndex = 0; mIndex < meshes.size(); mIndex++)
            {
                auto &mesh = meshes[mIndex];
                MeshData meshData{};
                meshData.index = mIndex;
                meshData.name = mesh->getName();

                const auto &meshVertices = mesh->getVertices();
                const auto &meshIndices = mesh->getIndices();
                meshData.vertexCount =
                    static_cast<uint32_t>(meshVertices.size());
                meshData.indexCount = static_cast<uint32_t>(meshIndices.size());

                auto bufferIndices =
                    createVertexAndIndexBuffer(meshVertices, meshIndices);
                if (bufferIndices.first == -1 || bufferIndices.second == -1)
                {
                    Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                      LogSeverity::trace,
                                      "Failed to put data in vertex/index "
                                      "buffer for mesh: " +
                                          mesh->getName());
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
                    mesh->getMaterial()->getName());
                if (matIndex == -1)
                {
                    matIndex = m_materialManager->getMaterialIndexFromName(
                        scene::Material::DEFAULT_MATERIAL_BASIC_SHADED
                            .getName());
                }
                meshData.materialIndex = static_cast<uint32_t>(matIndex);

                meshObject.meshes.emplace_back(std::move(meshData));
            }
            currMeshObjects.emplace_back(std::move(meshObject));
        }
    }
    return true;
}

void kirana::viewport::vulkan::SceneData::createObjectBuffer()
{
    // TODO: Make Object Data buffer device local.
    const vk::DeviceSize bufferSize =
        sizeof(vulkan::ObjectData) * m_scene.getSceneInfo().numMeshes;
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
    : m_isInitialized{false},
      m_isRaytracingInitialized{false}, m_device{device},
      m_allocator{allocator}, m_descriptorPool{descriptorPool},
      m_renderPass{renderPass}, m_raytraceData{raytraceData}, m_scene{scene},
      m_materialManager{new MaterialManager(m_device, m_allocator)}
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

    createMaterials(true);
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

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Destroying scene data...");
    m_scene.removeOnSceneLoadedEventListener(m_sceneLoadListener);
    m_scene.removeOnWorldChangeEventListener(m_worldChangeListener);
    m_scene.removeOnCameraChangeEventListener(m_cameraChangeListener);

    for (auto &v : m_vertexBuffers)
    {
        m_allocator->free(v.buffer);
    }
    for (auto &i : m_indexBuffers)
    {
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

    m_onSceneDataChange.removeAllListeners();

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Scene data destroyed");
}

void kirana::viewport::vulkan::SceneData::setShadingPipeline(
    vulkan::ShadingPipeline pipeline)
{
    m_currentShadingPipeline = pipeline;
    if (m_currentShadingPipeline == ShadingPipeline::RAYTRACE &&
        !m_isRaytracingInitialized)
        m_isRaytracingInitialized = m_raytraceData->initialize(*this);
    m_onSceneDataChange();
}

void kirana::viewport::vulkan::SceneData::setShadingType(
    vulkan::ShadingType type)
{
    m_currentShadingType = type;
    onObjectChanged();
}

const std::vector<kirana::viewport::vulkan::Texture *>
    &kirana::viewport::vulkan::SceneData::getTextures() const
{
    return m_materialManager->getTextures();
}

uint32_t kirana::viewport::vulkan::SceneData::getCurrentMaterialIndex(
    bool isEditorMesh, bool outline, uint32_t objIndex,
    uint32_t meshIndex) const
{
    if (isEditorMesh)
        return m_editorMeshes[objIndex].meshes[meshIndex].materialIndex;
    if (outline)
        return m_materialManager->getMaterialIndexFromName(
            scene::Material::DEFAULT_MATERIAL_EDITOR_OUTLINE.getName());

    switch (m_currentShadingType)
    {
    case ShadingType::PBR:
        return m_sceneMeshes[objIndex].meshes[meshIndex].materialIndex;
    case ShadingType::WIREFRAME:
        return m_materialManager->getMaterialIndexFromName(
            scene::Material::DEFAULT_MATERIAL_WIREFRAME.getName());
    default:
    case ShadingType::BASIC:
        return m_materialManager->getMaterialIndexFromName(
            scene::Material::DEFAULT_MATERIAL_BASIC_SHADED.getName());
    }
}

const kirana::viewport::vulkan::Pipeline &kirana::viewport::vulkan::SceneData::
    getCurrentPipeline(bool isEditorMesh, bool outline, uint32_t objIndex,
                       uint32_t meshIndex) const
{
    const uint32_t matIndex =
        getCurrentMaterialIndex(isEditorMesh, outline, objIndex, meshIndex);
    return *m_materialManager->getPipeline(matIndex, m_currentShadingPipeline);
}

const kirana::viewport::vulkan::ShaderBindingTable &kirana::viewport::vulkan::
    SceneData::getCurrentSBT(uint32_t objIndex, uint32_t meshIndex) const
{
    const uint32_t matIndex =
        getCurrentMaterialIndex(false, false, objIndex, meshIndex);
    return *m_materialManager->getShaderBindingTable(matIndex);
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
    bool isEditor, bool outline, uint32_t objIndex, uint32_t meshIndex,
    uint32_t instanceIndex) const
{
    const MeshObjectData &meshObjectData =
        isEditor ? m_editorMeshes[objIndex] : m_sceneMeshes[objIndex];
    const MeshData &meshData = meshObjectData.meshes[meshIndex];
    const uint32_t matIndex =
        getCurrentMaterialIndex(isEditor, outline, objIndex, meshIndex);

    return PushConstant<PushConstantRaster>(
        {meshObjectData.instances[instanceIndex].transform->getMatrix(),
         getVertexBufferAddress(meshData.vertexBufferIndex),
         getIndexBufferAddress(meshData.indexBufferIndex),
         m_materialManager->getMaterialDataBufferAddress(matIndex),
         m_materialManager->getMaterialDataIndex(matIndex)},
        vulkan::PUSH_CONSTANT_RASTER_SHADER_STAGES);
}

kirana::viewport::vulkan::PushConstant<
    kirana::viewport::vulkan::PushConstantRaytrace>
kirana::viewport::vulkan::SceneData::getPushConstantRaytraceData() const
{
    return PushConstant<PushConstantRaytrace>(
        {0, constants::VULKAN_RAYTRACING_MAX_BOUNCES,
         constants::VULKAN_RAYTRACING_AA_MULTIPLIER},
        vulkan::PUSH_CONSTANT_RAYTRACE_SHADER_STAGES);
}