#include "scene_data.hpp"
#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"
#include "allocator.hpp"
#include "shader.hpp"
#include "descriptor_set_layout.hpp"
#include "pipeline_layout.hpp"
#include "pipeline.hpp"

#include <algorithm>
#include <scene.hpp>

void kirana::viewport::vulkan::SceneData::setVertexDescription()
{
    vk::VertexInputBindingDescription bindingDesc(0, sizeof(scene::Vertex));
    m_vertexDesc.bindings.push_back(bindingDesc);

    vk::VertexInputAttributeDescription posAttrib(
        0, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, position));
    vk::VertexInputAttributeDescription normalAttrib(
        1, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, normal));
    vk::VertexInputAttributeDescription colorAttrib(
        2, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, color));

    m_vertexDesc.attributes.push_back(posAttrib);
    m_vertexDesc.attributes.push_back(normalAttrib);
    m_vertexDesc.attributes.push_back(colorAttrib);
}


const kirana::viewport::vulkan::Shader *kirana::viewport::vulkan::SceneData::
    createShader(const std::string &shaderName)
{
    if (m_shaders.find(shaderName) != m_shaders.end())
        return m_shaders.at(shaderName).get();
    else
    {
        std::unique_ptr<Shader> &val = m_shaders[shaderName];
        val = std::make_unique<Shader>(m_device, shaderName);
        return m_shaders[shaderName].get();
    }
}

kirana::viewport::vulkan::MaterialData kirana::viewport::vulkan::SceneData::
    getMaterialData(const scene::Material &material)
{
    MaterialData matData;
    const Shader *shader = createShader(material.getShader());

    const scene::Material::MaterialProperties &prop = material.getProperties();

    matData.properties = {
        vk::PrimitiveTopology::eTriangleList,
        prop.renderWireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill,
        vk::CullModeFlags(static_cast<VkCullModeFlagBits>(prop.cullMode)),
        prop.wireframeWidth,
        vk::SampleCountFlagBits::e1,
        prop.surfaceType == scene::Material::SurfaceType::Transparent};

    matData.name = material.getName();
    matData.shaderName = material.getShader();
    matData.layout =
        std::make_unique<PipelineLayout>(m_device, m_globalDescSetLayout);
    matData.pipeline = std::make_unique<Pipeline>(
        m_device, m_renderPass, shader, matData.layout.get(), m_vertexDesc,
        matData.properties);

    return matData;
}

void kirana::viewport::vulkan::SceneData::createMaterials()
{
    // Create internal viewport materials (MatCap, grid, etc.)
    const auto &mats = scene::Material::getDefaultMaterials();
    for (const auto &m : mats)
        m_materials[m.getName()] = std::move(getMaterialData(m));

    switch (m_currentShading)
    {
    case 2:
        // TODO: Add PBR Pipeline
    case 3:
        // TODO: Add Raytrace PBR Pipeline
        for (const auto &m : m_scene.getMaterials())
            m_materials[m->getName()] = std::move(getMaterialData(*m.get()));
        break;
    default:
        break;
    }
}

kirana::viewport::vulkan::MaterialData &kirana::viewport::vulkan::SceneData::
    findMaterial(const std::string &materialName, bool overrideShading)
{
    // Check if default material
    if (overrideShading)
        return m_materials[materialName];

    switch (m_currentShading)
    {
    case 0:
        return m_materials[constants::DEFAULT_MATERIAL_MAT_CAP_NAME];
    case 1:
        return m_materials[constants::DEFAULT_MATERIAL_WIREFRAME_NAME];
    default:
    case 2:
        // TODO: Add PBR Pipeline
    case 3:
        // TODO: Add Raytrace PBR Pipeline
        return m_materials[materialName];
    }
}

void kirana::viewport::vulkan::SceneData::createCameraBuffer()
{
    vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(vulkan::CameraData));
    vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
            vma::MemoryUsage::eCpuToGpu, &m_cameraBuffer))
    {
        m_cameraBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_cameraBuffer.buffer, 0, sizeof(vulkan::CameraData));
        onCameraChanged();
    }
}

void kirana::viewport::vulkan::SceneData::createWorldDataBuffer()
{
    vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData));
    vk::DeviceSize bufferSize =
        constants::VULKAN_FRAME_OVERLAP_COUNT * paddedSize;
    if (m_allocator->allocateBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
            vma::MemoryUsage::eCpuToGpu, &m_worldDataBuffer))
    {
        m_worldDataBuffer.descInfo = vk::DescriptorBufferInfo(
            *m_worldDataBuffer.buffer, 0, sizeof(scene::WorldData));
        onWorldChanged();
    }
}

bool kirana::viewport::vulkan::SceneData::createViewportMeshes()
{
    bool initialized = true;
    for (const auto &o : m_scene.getViewportObjects())
    {
        // TODO: Calculate offset based on previous index MeshData.
        const auto &mesh = o->getMeshes()[0];
        const auto &material = mesh->getMaterial();
        const auto &vertices = mesh->getVertices();

        MeshData meshData;
        meshData.vertexCount = vertices.size();
        // TODO: Find instances for each mesh
        meshData.instances.resize(1);
        meshData.instances[0].transform = o->transform;

        meshData.material = &findMaterial(material->getName(), true);

        size_t verticesSize = vertices.size() * sizeof(scene::Vertex);
        if (!m_allocator->allocateBufferToGPU(
                verticesSize, vk::BufferUsageFlagBits::eVertexBuffer,
                &meshData.vertexBuffer, vertices.data()))
            initialized = false;

        const std::vector<uint32_t> &indices = mesh->getIndices();
        meshData.indexCount = indices.size();
        size_t indicesSize = indices.size() * sizeof(uint32_t);
        if (!m_allocator->allocateBufferToGPU(
                indicesSize, vk::BufferUsageFlagBits::eIndexBuffer,
                &meshData.indexBuffer, indices.data()))
            initialized = false;

        if (initialized)
            m_meshes.emplace_back(std::move(meshData));
    }
    return initialized;
}

bool kirana::viewport::vulkan::SceneData::createSceneMeshes()
{
    bool initialized = true;
    for (const auto &m : m_scene.getMeshes())
    {
        // TODO: Calculate offset based on previous index MeshData.
        MeshData meshData;

        const auto &vertices = m->getVertices();
        const auto &transforms = m_scene.getTransformsForMesh(m.get());

        meshData.vertexCount = vertices.size();

        meshData.instances.resize(transforms.size());
        for (size_t i = 0; i < transforms.size(); i++)
            meshData.instances[i] = {transforms[i]};

        meshData.material = &findMaterial(m->getMaterial()->getName());

        size_t verticesSize = vertices.size() * sizeof(scene::Vertex);
        if (!m_allocator->allocateBufferToGPU(
                verticesSize, vk::BufferUsageFlagBits::eVertexBuffer,
                &meshData.vertexBuffer, vertices.data()))
            initialized = false;

        const std::vector<uint32_t> &indices = m->getIndices();
        meshData.indexCount = indices.size();
        size_t indicesSize = indices.size() * sizeof(uint32_t);
        if (!m_allocator->allocateBufferToGPU(
                indicesSize, vk::BufferUsageFlagBits::eIndexBuffer,
                &meshData.indexBuffer, indices.data()))
            initialized = false;

        if (initialized)
            m_meshes.emplace_back(std::move(meshData));
    }
    return initialized;
}

void kirana::viewport::vulkan::SceneData::onCameraChanged()
{
    const auto &camera = m_scene.getActiveCamera();
    m_cameraData.viewMatrix = camera->getViewMatrix();
    m_cameraData.projectionMatrix = camera->getProjectionMatrix();
    m_cameraData.viewProjectionMatrix =
        m_cameraData.projectionMatrix * m_cameraData.viewMatrix;
    m_cameraData.position = camera->transform.getPosition();
    m_cameraData.direction = camera->transform.getForward();
    m_cameraData.nearPlane = camera->nearPlane;
    m_cameraData.farPlane = camera->farPlane;

    vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(vulkan::CameraData));
    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->mapToMemory(m_cameraBuffer, sizeof(vulkan::CameraData),
                                 static_cast<uint32_t>(paddedSize * i),
                                 &m_cameraData);
    }
}

void kirana::viewport::vulkan::SceneData::onWorldChanged()
{
    vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData));
    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->mapToMemory(m_worldDataBuffer, sizeof(scene::WorldData),
                                 static_cast<uint32_t>(paddedSize * i),
                                 &m_scene.getWorldData());
    }
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const RenderPass *renderPass,
    const DescriptorSetLayout *globalDescSetLayout, scene::Scene &scene,
    uint16_t shadingIndex)
    : m_isInitialized{false}, m_currentShading{shadingIndex}, m_device{device},
      m_allocator{allocator}, m_renderPass{renderPass},
      m_globalDescSetLayout{globalDescSetLayout}, m_scene{scene}
{
    m_isInitialized = true;

    // Set the vulkan description of vertex buffers.
    setVertexDescription();

    // Create shaders, pipeline layouts and pipelines for all the materials in
    // the scene.
    m_materials.clear();
    m_shaders.clear();
    createMaterials();
    // Create Camera data buffer.
    if (m_isInitialized)
        createCameraBuffer();
    // Create World data buffer.
    createWorldDataBuffer();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_meshes.clear();
    m_isInitialized = createViewportMeshes();
    m_isInitialized = createSceneMeshes();
    if (m_isInitialized)
    {
        m_cameraChangeListener =
            m_scene.getActiveCamera()->addOnCameraChangeEventListener(
                [&]() { this->onCameraChanged(); });
        m_worldChangeListener = m_scene.addOnWorldChangeEventListener(
            [&]() { this->onWorldChanged(); });

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
    m_scene.removeOnWorldChangeEventListener(m_worldChangeListener);
    m_scene.getActiveCamera()->removeOnCameraChangeEventListener(
        m_cameraChangeListener);

    if (m_cameraBuffer.buffer)
        m_allocator->free(m_cameraBuffer);
    if (m_worldDataBuffer.buffer)
        m_allocator->free(m_worldDataBuffer);
    if (!m_meshes.empty())
    {
        for (const auto &m : m_meshes)
        {
            m_allocator->free(m.indexBuffer);
            m_allocator->free(m.vertexBuffer);
        }
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Scene data destroyed");
    }
}

void kirana::viewport::vulkan::SceneData::setShading(uint16_t shadingIndex)
{
    m_currentShading = shadingIndex;
    rebuildPipeline(m_renderPass);
}

void kirana::viewport::vulkan::SceneData::rebuildPipeline(
    const RenderPass *renderPass)
{
    m_renderPass = renderPass;
    for (auto &m : m_materials)
    {
        m.second.pipeline = std::make_unique<Pipeline>(
            m_device, m_renderPass, m_shaders[m.second.shaderName].get(),
            m.second.layout.get(), m_vertexDesc, m.second.properties);
    }
}

const kirana::viewport::vulkan::AllocatedBuffer &kirana::viewport::vulkan::
    SceneData::getCameraBuffer() const
{
    return m_cameraBuffer;
}

uint32_t kirana::viewport::vulkan::SceneData::getCameraBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(
        vulkan::padUniformBufferSize(m_device->gpu,
                                     sizeof(vulkan::CameraData)) *
        offsetIndex);
}

const kirana::viewport::vulkan::AllocatedBuffer &kirana::viewport::vulkan::
    SceneData::getWorldDataBuffer() const
{
    return m_worldDataBuffer;
}

uint32_t kirana::viewport::vulkan::SceneData::getWorldDataBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData)) *
        offsetIndex);
}