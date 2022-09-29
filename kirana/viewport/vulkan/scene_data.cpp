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
    m_meshes.clear();
    size_t verticesSize;
    for (const auto &m : scene.meshes)
    {
        // TODO: Calculate offset based on previous index MeshData.
        MeshData meshData;
        meshData.vertexCount = m->vertices.size();
        verticesSize = m->vertices.size() * sizeof(scene::Vertex);
        if (m_allocator->allocate(
                verticesSize, vk::BufferUsageFlagBits::eVertexBuffer,
                vma::MemoryUsage::eCpuToGpu, &meshData.vertexBuffer))
        {
            m_meshes.emplace_back(std::move(meshData));
            if (m_allocator->mapToMemory(m_meshes.back().vertexBuffer,
                                         verticesSize, m->vertices.data()))
            {
                m_isInitialized = true;
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::debug,
                                  "Generated scene data for viewport");
            }
        }
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