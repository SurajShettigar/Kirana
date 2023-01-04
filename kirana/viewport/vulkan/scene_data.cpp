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
#include "acceleration_structure.hpp"

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
        2, 0, vk::Format::eR32G32B32Sfloat, offsetof(scene::Vertex, color));

    m_vertexDesc.attributes.push_back(posAttrib);
    m_vertexDesc.attributes.push_back(normalAttrib);
    m_vertexDesc.attributes.push_back(colorAttrib);
}

void kirana::viewport::vulkan::SceneData::onWorldChanged()
{
    const vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData));

    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToMemory(
            m_worldDataBuffer, sizeof(scene::WorldData),
            static_cast<uint32_t>(paddedSize * i), &m_scene.getWorldData());
    }
}

void kirana::viewport::vulkan::SceneData::onCameraChanged()
{
    const auto &camera = m_scene.getCamera();
    m_cameraData.viewMatrix = camera.getViewMatrix();
    m_cameraData.projectionMatrix = camera.getProjectionMatrix();
    m_cameraData.viewProjectionMatrix =
        m_cameraData.projectionMatrix * m_cameraData.viewMatrix;
    m_cameraData.position = camera.transform.getPosition();
    m_cameraData.direction = camera.transform.getForward();
    m_cameraData.nearPlane = camera.nearPlane;
    m_cameraData.farPlane = camera.farPlane;

    const vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(vulkan::CameraData));
    for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        m_allocator->copyDataToMemory(
            m_cameraBuffer, sizeof(vulkan::CameraData),
            static_cast<uint32_t>(paddedSize * i), &m_cameraData);
    }
}

void kirana::viewport::vulkan::SceneData::onObjectChanged()
{

    auto data = (vulkan::ObjectData *)m_objectBuffer.memoryPointer;
    math::Matrix4x4 mat;
    size_t index = 0;
    for (size_t frameIndex = 0;
         frameIndex < constants::VULKAN_FRAME_OVERLAP_COUNT; frameIndex++)
    {
        for (const auto &m : m_meshes)
        {
            for (const auto &i : m.second.instances)
            {
                mat = i.transform->getMatrix();
                data[index].modelMatrix = mat;
                ++index;
            }
        }
    }
}

void kirana::viewport::vulkan::SceneData::createWorldDataBuffer()
{
    const vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData));
    const vk::DeviceSize bufferSize =
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

void kirana::viewport::vulkan::SceneData::createCameraBuffer()
{
    const vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(vulkan::CameraData));
    const vk::DeviceSize bufferSize =
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
        prop.surfaceType == scene::Material::SurfaceType::TRANSPARENT,
        prop.enableDepth,
        prop.writeDepth,
        static_cast<vk::CompareOp>(prop.depthCompareOp),
        prop.stencil.enableTest,
        static_cast<vk::CompareOp>(prop.stencil.compareOp),
        static_cast<vk::StencilOp>(prop.stencil.failOp),
        static_cast<vk::StencilOp>(prop.stencil.depthFailOp),
        static_cast<vk::StencilOp>(prop.stencil.passOp),
        prop.stencil.reference

    };

    matData.name = material.getName();
    matData.shaderName = material.getShader();
    const std::vector<const DescriptorSetLayout *> descLayouts{
        m_globalDescSetLayout, m_objectDescSetLayout};
    matData.layout = std::make_unique<PipelineLayout>(m_device, descLayouts);
    matData.pipeline = std::make_unique<Pipeline>(
        m_device, m_renderPass, shader, matData.layout.get(), m_vertexDesc,
        matData.properties);

    return matData;
}

void kirana::viewport::vulkan::SceneData::createMaterials()
{
    for (const auto &m : m_scene.getRenderableMaterials())
        m_materials[m->getName()] = std::move(getMaterialData(*m));
}

kirana::viewport::vulkan::MaterialData &kirana::viewport::vulkan::SceneData::
    findMaterial(const std::string &materialName, bool overrideShading)
{
    if (overrideShading)
        return m_materials[materialName];

    switch (m_currentShading)
    {
    case viewport::Shading::BASIC:
        return m_materials[constants::DEFAULT_MATERIAL_MAT_CAP_NAME];
    case viewport::Shading::WIREFRAME:
        return m_materials[constants::DEFAULT_MATERIAL_WIREFRAME_NAME];
    default:
    case viewport::Shading::REALTIME_PBR:
        // TODO: Add PBR Pipeline
    case viewport::Shading::RAYTRACE_PBR:
        // TODO: Add Raytrace PBR Pipeline
        return m_materials[materialName];
    }
}

bool kirana::viewport::vulkan::SceneData::createMeshes()
{
    bool initialized = true;
    m_totalInstanceCount = 0;
    uint32_t meshIndex = 0;
    uint32_t instanceIndex = 0;
    for (const auto &r : m_scene.getRenderables())
    {
        InstanceData instance{instanceIndex++, r.object->transform, &r.selected,
                              &r.renderVisible};
        for (const auto &m : r.object->getMeshes())
        {
            const auto &meshName = m->getName();
            if (m_meshes.find(meshName) != m_meshes.end())
                m_meshes[meshName].instances.emplace_back(instance);
            else
            {
                // Mesh was found for the first time.

                // Reset instance Index
                instanceIndex = 0;
                instance.index = instanceIndex;

                // Create Mesh Data
                const auto &vertices = m->getVertices();
                const auto &indices = m->getIndices();
                const auto &material = m->getMaterial();

                MeshData &meshData = m_meshes[meshName];
                // Make it non-renderable at first
                meshData.render = false;
                // Assign index
                meshData.index = meshIndex++;
                // Assign material
                meshData.material =
                    &findMaterial(material->getName(), r.overrideMaterial);
                // Assign instance data
                meshData.instances.emplace_back(instance);
                // Create vertex buffer
                meshData.vertexCount = vertices.size();
                const size_t verticesSize =
                    meshData.vertexCount * sizeof(scene::Vertex);
                if (!m_allocator->allocateBufferToGPU(
                        verticesSize,
                        vk::BufferUsageFlagBits::eVertexBuffer |
                            vk::BufferUsageFlagBits::eShaderDeviceAddress |
                            vk::BufferUsageFlagBits::
                                eAccelerationStructureBuildInputReadOnlyKHR |
                            vk::BufferUsageFlagBits::eStorageBuffer,
                        &meshData.vertexBuffer, vertices.data()))
                    initialized = false;
                // Create index buffer
                meshData.indexCount = indices.size();
                const size_t indicesSize =
                    meshData.indexCount * sizeof(uint32_t);
                if (!m_allocator->allocateBufferToGPU(
                        indicesSize,
                        vk::BufferUsageFlagBits::eIndexBuffer |
                            vk::BufferUsageFlagBits::eShaderDeviceAddress |
                            vk::BufferUsageFlagBits::
                                eAccelerationStructureBuildInputReadOnlyKHR |
                            vk::BufferUsageFlagBits::eStorageBuffer,
                        &meshData.indexBuffer, indices.data()))
                    initialized = false;
            }
            // If any one of the instance is visible, render it.
            if (r.renderVisible)
                m_meshes[meshName].render = true;

            ++m_totalInstanceCount;
        }
    }
    return initialized;
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
        onObjectChanged();
        return true;
    }
    return false;
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const RenderPass *renderPass, const scene::ViewportScene &scene,
    viewport::Shading shading)
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

    // Create shaders, pipeline layouts and pipelines for all the materials in
    // the scene.
    m_materials.clear();
    m_shaders.clear();
    createMaterials();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_meshes.clear();
    m_isInitialized = createMeshes();
    m_isInitialized = createObjectBuffer();
    if (m_isInitialized)
    {
        m_accelStructure =
            new AccelerationStructure(m_device, m_allocator, m_meshes);

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
    m_scene.removeOnCameraChangeEventListener(m_cameraChangeListener);

    if (m_accelStructure)
    {
        delete m_accelStructure;
        m_accelStructure = nullptr;
    }

    if (m_objectBuffer.buffer)
    {
        m_allocator->free(m_objectBuffer);
    }
    if (m_worldDataBuffer.buffer)
    {
        m_allocator->free(m_worldDataBuffer);
    }
    if (m_cameraBuffer.buffer)
    {
        m_allocator->free(m_cameraBuffer);
    }
    if (!m_meshes.empty())
    {
        for (const auto &m : m_meshes)
        {
            m_allocator->free(m.second.indexBuffer);
            m_allocator->free(m.second.vertexBuffer);
        }
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Scene data destroyed");
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
}

const vk::AccelerationStructureKHR &kirana::viewport::vulkan::SceneData::
    getAccelerationStructure() const
{
    return m_accelStructure->getAccelerationStructure();
}

const kirana::viewport::vulkan::MaterialData &kirana::viewport::vulkan::
    SceneData::getOutlineMaterial() const
{
    return m_materials[std::string(constants::DEFAULT_MATERIAL_OUTLINE_NAME)];
}

void kirana::viewport::vulkan::SceneData::setShading(viewport::Shading shading)
{
    m_currentShading = shading;
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


const kirana::viewport::vulkan::AllocatedBuffer &kirana::viewport::vulkan::
    SceneData::getObjectBuffer() const
{
    return m_objectBuffer;
}

uint32_t kirana::viewport::vulkan::SceneData::getObjectBufferOffset(
    uint32_t offsetIndex) const
{
    return static_cast<uint32_t>(sizeof(vulkan::ObjectData) *
                                 m_totalInstanceCount * offsetIndex);
}