#include "raytrace_data.hpp"

#include "device.hpp"
#include "allocator.hpp"
#include "swapchain.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "acceleration_structure.hpp"
#include "texture.hpp"
#include "scene_data.hpp"

const vk::BufferUsageFlagBits
    kirana::viewport::vulkan::RaytraceData::VERTEX_INDEX_BUFFER_USAGE_FLAGS =
        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;

bool kirana::viewport::vulkan::RaytraceData::createRenderTarget()
{
    m_renderTarget = new Texture(
        m_device, m_allocator,
        Texture::Properties{{m_swapchain->getSurfaceResolution()[0],
                             m_swapchain->getSurfaceResolution()[1], 1},
                            m_swapchain->imageFormat,
                            vk::ImageUsageFlagBits::eStorage |
                                vk::ImageUsageFlagBits::eTransferSrc,
                            vk::ImageAspectFlagBits::eColor,
                            true,
                            vk::ImageLayout::eGeneral},
        "Raytrace_Target");
    return m_renderTarget->isInitialized;
}

kirana::viewport::vulkan::RaytraceData::RaytraceData(const Device *device,
                                                     const Allocator *allocator,
                                                     const Swapchain *swapchain)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_swapchain{swapchain}
{
    m_isInitialized = createRenderTarget();
}

kirana::viewport::vulkan::RaytraceData::~RaytraceData()
{
    if (m_accelStruct)
    {
        delete m_accelStruct;
        m_accelStruct = nullptr;
    }
    if (m_renderTarget)
    {
        delete m_renderTarget;
        m_renderTarget = nullptr;
    }
}

bool kirana::viewport::vulkan::RaytraceData::buildAccelerationStructure(
    const SceneData &sceneData)
{
    m_accelStruct = new AccelerationStructure(m_device, m_allocator,
                                              sceneData.getMeshData());
}

void kirana::viewport::vulkan::RaytraceData::setPipeline(
    const RaytracePipeline *pipeline, const ShaderBindingTable *sbt)
{
    m_currentPipeline = pipeline;
    m_currentSBT = sbt;
}

void kirana::viewport::vulkan::RaytraceData::rebuildRenderTarget(
    const Swapchain *newSwapchain)
{
    m_swapchain = newSwapchain;
    if (m_renderTarget)
    {
        delete m_renderTarget;
        m_renderTarget = nullptr;
    }
    createRenderTarget();
}