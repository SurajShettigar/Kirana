#include "scene_data.hpp"
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"
#include "allocator.hpp"
#include "shader.hpp"
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
        matData.layout = std::make_unique<PipelineLayout>(m_device);
        matData.pipeline = std::make_unique<Pipeline>(
            m_device, m_renderPass, shader, matData.layout.get(), m_vertexDesc,
            m_windowResolution);

        m_materials.emplace_back(std::move(matData));
    }
}

kirana::viewport::vulkan::SceneData::SceneData(
    const Device *device, const RenderPass *renderPass,
    const Allocator *allocator, const std::array<int, 2> windowResolution,
    const scene::Scene &scene)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_allocator{allocator}, m_windowResolution{windowResolution}, m_scene{
                                                                        scene}
{
    // Set the vulkan description of vertex buffers.
    setVertexDescription();
    // Create shaders, pipeline layouts and pipelines for all the materials in
    // the scene.
    createMaterials();

    // Create vertex buffers and map it to memory for each mesh of the scene.
    m_isInitialized = true;
    m_meshes.clear();
    for (const auto &m : m_scene.getMeshes())
    {
        // TODO: Calculate offset based on previous index MeshData.
        MeshData meshData;
        const std::vector<scene::Vertex> &vertices = m->getVertices();
        meshData.vertexCount = vertices.size();
        meshData.instanceTransforms =
            std::move(m_scene.getTransformsForMesh(m.get()));

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
                          "Scene data destroyed");
    }
}

kirana::math::Matrix4x4 kirana::viewport::vulkan::SceneData::getClipSpaceMatrix(
    size_t meshIndex, size_t instanceIndex) const
{
    return math::Matrix4x4::transpose(m_scene.getClipSpaceMatrix(
        m_meshes[meshIndex].instanceTransforms[instanceIndex]));
}