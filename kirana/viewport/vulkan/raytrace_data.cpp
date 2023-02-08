#include "raytrace_data.hpp"

#include "device.hpp"
#include "allocator.hpp"
#include "swapchain.hpp"
#include "pipeline_layout.hpp"
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"
#include "descriptor_set.hpp"
#include "raytrace_pipeline.hpp"
#include "shader_binding_table.hpp"
#include "acceleration_structure.hpp"
#include "texture.hpp"
#include "scene_data.hpp"

const vk::BufferUsageFlagBits
    kirana::viewport::vulkan::RaytraceData::VERTEX_INDEX_BUFFER_USAGE_FLAGS =
        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;

void kirana::viewport::vulkan::RaytraceData::bindDescriptorSets(
    const SceneData &sceneData)
{
    DescriptorBindingInfo bindingInfo =
        DescriptorSetLayout::getBindingInfoForData(
            DescriptorBindingDataType::CAMERA, ShadingPipeline::RAYTRACE);

    m_descSets[static_cast<int>(bindingInfo.layoutType)].bindBuffer(
        bindingInfo, sceneData.getCameraBuffer());

    bindingInfo = DescriptorSetLayout::getBindingInfoForData(
        DescriptorBindingDataType::WORLD, ShadingPipeline::RAYTRACE);

    m_descSets[static_cast<int>(bindingInfo.layoutType)].bindBuffer(
        bindingInfo, sceneData.getWorldDataBuffer());

    bindingInfo = DescriptorSetLayout::getBindingInfoForData(
        DescriptorBindingDataType::OBJECT_DATA, ShadingPipeline::RAYTRACE);

    m_descSets[static_cast<int>(bindingInfo.layoutType)].bindBuffer(
        bindingInfo, sceneData.getObjectDataBuffer());
}

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

    if (m_renderTarget->isInitialized)
    {
        const auto &bindingInfo = DescriptorSetLayout::getBindingInfoForData(
            DescriptorBindingDataType::RAYTRACE_RENDER_TARGET,
            ShadingPipeline::RAYTRACE);
        m_descSets[static_cast<int>(bindingInfo.layoutType)].bindImage(
            bindingInfo, *m_renderTarget);
    }

    return m_renderTarget->isInitialized;
}

bool kirana::viewport::vulkan::RaytraceData::createAccelerationStructure(
    const SceneData &sceneData)
{
    m_accelStruct = new AccelerationStructure(m_device, m_allocator, sceneData);
    if (m_accelStruct->isInitialized)
    {
        const DescriptorBindingInfo bindingInfo =
            DescriptorSetLayout::getBindingInfoForData(
                DescriptorBindingDataType::RAYTRACE_ACCEL_STRUCT,
                ShadingPipeline::RAYTRACE);
        m_descSets[static_cast<int>(bindingInfo.layoutType)]
            .bindAccelerationStructure(bindingInfo, *m_accelStruct);
    }
    return m_accelStruct->isInitialized;
}

kirana::viewport::vulkan::RaytraceData::RaytraceData(
    const Device *device, const Allocator *allocator,
    const DescriptorPool *const descriptorPool, const Swapchain *swapchain)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_descriptorPool{descriptorPool}, m_swapchain{swapchain}
{
    bool init = PipelineLayout::getDefaultPipelineLayout(
        m_device, vulkan::ShadingPipeline::RAYTRACE, m_raytracePipelineLayout);
    if (init)
    {
        const auto &descLayouts =
            m_raytracePipelineLayout->getDescriptorSetLayouts();
        m_descSets.resize(descLayouts.size());
        m_descriptorPool->allocateDescriptorSets(descLayouts, &m_descSets);
    }
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
    if (m_raytracePipelineLayout)
    {
        delete m_raytracePipelineLayout;
        m_raytracePipelineLayout = nullptr;
    }
}

bool kirana::viewport::vulkan::RaytraceData::initialize(
    const SceneData &sceneData)
{
    bindDescriptorSets(sceneData);
    m_isInitialized = createAccelerationStructure(sceneData);
    if(!m_isInitialized)
        return false;
    m_isInitialized = createRenderTarget();
    if(!m_isInitialized)
        return false;
    updateDescriptors();
    m_isInitialized = true;

    return m_isInitialized;
}

void kirana::viewport::vulkan::RaytraceData::updateDescriptors(int setIndex)
{
    if (setIndex == -1)
        for (const auto &s : m_descSets)
            m_descriptorPool->writeDescriptorSet(s);
    else
        m_descriptorPool->writeDescriptorSet(m_descSets[setIndex]);
}

void kirana::viewport::vulkan::RaytraceData::setPipeline(
    const RaytracePipeline *pipeline, const ShaderBindingTable *sbt)
{
    m_currentPipeline = pipeline;
    m_currentSBT = sbt;
}

void kirana::viewport::vulkan::RaytraceData::rebuildRenderTarget()
{
    if (m_renderTarget)
    {
        delete m_renderTarget;
        m_renderTarget = nullptr;
    }
    createRenderTarget();
    updateDescriptors(static_cast<int>(vulkan::DescriptorLayoutType::GLOBAL));
}