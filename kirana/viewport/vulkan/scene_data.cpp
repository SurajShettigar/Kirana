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
    auto it = std::find_if(m_shaders.begin(), m_shaders.end(),
                           [&shaderName](const std::unique_ptr<Shader> &s) {
                               return s->name == shaderName;
                           });

    if (it != m_shaders.end())
        return (*it).get();
    else
    {
        m_shaders.emplace_back(std::make_unique<Shader>(m_device, shaderName));
        return m_shaders.back().get();
    }
}

void kirana::viewport::vulkan::SceneData::createMaterials()
{
    m_materials.clear();
    m_shaders.clear();
    for (const auto &m : m_scene.getMaterials())
    {
        const Shader *shader = createShader(m->getShader());

        MaterialData matData;
        matData.name = m->getName();
        matData.shaderName = m->getShader();
        matData.layout =
            std::make_unique<PipelineLayout>(m_device, m_globalDescSetLayout);
        matData.pipeline = std::make_unique<Pipeline>(
            m_device, m_renderPass, shader, matData.layout.get(), m_vertexDesc,
            m_windowResolution);

        m_materials.emplace_back(std::move(matData));
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

        for (size_t i = 0; i < constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
        {
            m_allocator->mapToMemory(m_worldDataBuffer, sizeof(scene::WorldData),
                                     paddedSize * i, &m_scene.getWorldData());
        }
    }
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const Allocator *allocator,
    const RenderPass *renderPass,
    const DescriptorSetLayout *globalDescSetLayout,
    const std::array<int, 2> windowResolution, const scene::Scene &scene)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_renderPass{renderPass}, m_globalDescSetLayout{globalDescSetLayout},
      m_windowResolution{windowResolution}, m_scene{scene}
{
    // Set the vulkan description of vertex buffers.
    setVertexDescription();
    // Create shaders, pipeline layouts and pipelines for all the materials in
    // the scene.
    createMaterials();

    // Create World Data buffer.
    createWorldDataBuffer();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_isInitialized = true;
    m_meshes.clear();
    for (const auto &m : m_scene.getMeshes())
    {
        // TODO: Calculate offset based on previous index MeshData.
        MeshData meshData;
        const std::vector<scene::Vertex> &vertices = m->getVertices();
        meshData.vertexCount = vertices.size();
        const auto &transforms = m_scene.getTransformsForMesh(m.get());
        meshData.instances.resize(transforms.size());
        for (size_t i = 0; i < transforms.size(); i++)
            meshData.instances[i] = {transforms[i]};

        meshData.material =
            &(*std::find_if(m_materials.begin(), m_materials.end(),
                            [&m](const MaterialData &mat) {
                                return mat.name == m->getMaterial()->getName();
                            }));

        size_t verticesSize = vertices.size() * sizeof(scene::Vertex);
        if (m_allocator->allocateBuffer(
                verticesSize, vk::BufferUsageFlagBits::eVertexBuffer,
                vma::MemoryUsage::eCpuToGpu, &meshData.vertexBuffer))
        {
            if (!m_allocator->mapToMemory(meshData.vertexBuffer, verticesSize,
                                          0, vertices.data()))
            {
                m_isInitialized = false;
            }
        }
        else
            m_isInitialized = false;

        const std::vector<uint32_t> &indices = m->getIndices();
        meshData.indexCount = indices.size();
        size_t indicesSize = indices.size() * sizeof(uint32_t);
        if (m_allocator->allocateBuffer(
                indicesSize, vk::BufferUsageFlagBits::eIndexBuffer,
                vma::MemoryUsage::eCpuToGpu, &meshData.indexBuffer))
        {
            if (!m_allocator->mapToMemory(meshData.indexBuffer, indicesSize, 0,
                                          indices.data()))
            {
                m_isInitialized = false;
            }
        }
        else
            m_isInitialized = false;

        if (m_isInitialized)
            m_meshes.emplace_back(std::move(meshData));
    }
    if (m_isInitialized)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
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
    if (m_worldDataBuffer.buffer)
        m_allocator->free(m_worldDataBuffer);
    if (!m_meshes.empty())
    {
        for (const auto &m : m_meshes)
        {
            m_allocator->free(m.indexBuffer);
            m_allocator->free(m.vertexBuffer);
        }
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Scene data destroyed");
    }
}

const kirana::viewport::vulkan::CameraData &kirana::viewport::vulkan::
    SceneData::getCameraData() const
{
    m_cameraData.viewMatrix = m_scene.getCamera().transform.getMatrix();
    m_cameraData.projectionMatrix = m_scene.getCamera().projection.getMatrix();
    return m_cameraData;
}

const kirana::viewport::vulkan::AllocatedBuffer &kirana::viewport::vulkan::
    SceneData::getWorldDataBuffer() const
{
    return m_worldDataBuffer;
}

[[nodiscard]] vk::DeviceSize kirana::viewport::vulkan::SceneData::
    getWorldDataBufferOffset(uint32_t offsetIndex) const
{
    return vulkan::padUniformBufferSize(m_device->gpu,
                                        sizeof(scene::WorldData)) *
           offsetIndex;
}

void kirana::viewport::vulkan::SceneData::updateWorldDataBuffer(uint32_t offsetIndex) const
{
    vk::DeviceSize paddedSize =
        vulkan::padUniformBufferSize(m_device->gpu, sizeof(scene::WorldData));
    m_allocator->mapToMemory(m_worldDataBuffer, sizeof(scene::WorldData),
                             paddedSize * offsetIndex, &m_scene.getWorldData());
}