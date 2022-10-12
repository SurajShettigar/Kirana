#include "scene_data.hpp"
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"
#include "allocator.hpp"

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

kirana::viewport::vulkan::SceneData::SceneData(const Allocator *const allocator,
                                               const scene::Scene &scene)
    : m_isInitialized{false}, m_allocator{allocator}
{
    // Set the vulkan description of vertex buffers.
    setVertexDescription();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_isInitialized = true;
    m_meshes.clear();
    for (const auto &m : scene.getMeshes())
    {
        // TODO: Calculate offset based on previous index MeshData.
        MeshData meshData;
        const std::vector<scene::Vertex> &vertices = m->getVertices();
        meshData.vertexCount = vertices.size();
        meshData.instanceTransforms =
            std::move(scene.getTransformsForMesh(m.get()));

        size_t verticesSize = vertices.size() * sizeof(scene::Vertex);
        if (m_allocator->allocateBuffer(
                verticesSize, vk::BufferUsageFlagBits::eVertexBuffer,
                vma::MemoryUsage::eCpuToGpu, &meshData.vertexBuffer))
        {
            m_meshes.emplace_back(std::move(meshData));
            if (!m_allocator->mapToMemory(m_meshes.back().vertexBuffer,
                                          verticesSize, vertices.data()))
            {
                m_isInitialized = false;
            }
        }
        else
            m_isInitialized = false;
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
    if (!m_meshes.empty())
    {
        for (const auto &m : m_meshes)
            m_allocator->free(m.vertexBuffer);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Destroyed scene data");
    }
}