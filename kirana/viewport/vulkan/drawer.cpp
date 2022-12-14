#include "drawer.hpp"

#include "command_pool.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"
#include "descriptor_set.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "pipeline_layout.hpp"
#include "pipeline.hpp"
#include "scene_data.hpp"
#include "vulkan_utils.hpp"

#include <constants.h>

const kirana::viewport::vulkan::FrameData &kirana::viewport::vulkan::Drawer::
    getCurrentFrame() const
{
    return m_frames[m_currentFrameNumber %
                    utils::constants::VULKAN_FRAME_OVERLAP_COUNT];
}

uint32_t kirana::viewport::vulkan::Drawer::getCurrentFrameIndex() const
{
    return m_currentFrameNumber % utils::constants::VULKAN_FRAME_OVERLAP_COUNT;
}

kirana::viewport::vulkan::Drawer::Drawer(
    const Device *const device, const Allocator *const allocator,
    const DescriptorPool *const descriptorPool,
    const Swapchain *const swapchain, const RenderPass *const renderPass,
    const SceneData *const scene)
    : m_isInitialized{false}, m_currentFrameNumber{0}, m_device{device},
      m_allocator{allocator}, m_descriptorPool{descriptorPool},
      m_swapchain{swapchain}, m_renderPass{renderPass}, m_scene{scene}
{
    m_frames.resize(utils::constants::VULKAN_FRAME_OVERLAP_COUNT);
    for (size_t i = 0; i < utils::constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        try
        {
            bool setAllocated = m_descriptorPool->allocateDescriptorSets(
                {&m_frames[i].globalDescriptorSet,
                 &m_frames[i].objectDescriptorSet},
                {m_scene->getGlobalDescriptorSetLayout(),
                 m_scene->getObjectDescriptorSetLayout()});

            if (setAllocated)
            {
                m_frames[i].globalDescriptorSet->writeBuffer(
                    m_scene->getCameraBuffer().descInfo,
                    vk::DescriptorType::eUniformBufferDynamic, 0);
                m_frames[i].globalDescriptorSet->writeBuffer(
                    m_scene->getWorldDataBuffer().descInfo,
                    vk::DescriptorType::eUniformBufferDynamic, 1);

                m_frames[i].objectDescriptorSet->writeBuffer(
                    m_scene->getObjectBuffer().descInfo,
                    vk::DescriptorType::eStorageBufferDynamic, 0);
            }

            m_frames[i].renderFence = m_device->current.createFence(
                vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
            m_frames[i].renderSemaphore =
                m_device->current.createSemaphore(vk::SemaphoreCreateInfo());
            m_frames[i].presentSemaphore =
                m_device->current.createSemaphore(vk::SemaphoreCreateInfo());

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Synchronization structures initialized");

            m_frames[i].commandPool = new CommandPool(
                m_device, m_device->queueFamilyIndices.graphics);
            m_frames[i].commandPool->allocateCommandBuffers(
                m_frames[i].commandBuffers);
            m_isInitialized = true;
        }
        catch (...)
        {
            handleVulkanException();
        }
    }
}

kirana::viewport::vulkan::Drawer::~Drawer()
{
    m_onSwapchainOutOfDate.removeAllListeners();
    if (m_device)
    {
        if (!m_frames.empty())
        {
            for (const auto &f : m_frames)
            {
                if (f.renderFence)
                    VK_HANDLE_RESULT(m_device->current.waitForFences(
                                         f.renderFence, true,
                                         constants::VULKAN_FRAME_SYNC_TIMEOUT),
                                     "Failed to wait for render fence");
                if (f.globalDescriptorSet)
                    delete f.globalDescriptorSet;
                if (f.renderFence)
                    m_device->current.destroyFence(f.renderFence);
                if (f.renderSemaphore)
                    m_device->current.destroySemaphore(f.renderSemaphore);
                if (f.presentSemaphore)
                    m_device->current.destroySemaphore(f.presentSemaphore);

                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::trace,
                                  "Synchronization structures destroyed");

                if (f.commandPool)
                {
                    delete f.commandPool;
                }
                if (f.commandBuffers)
                {
                    delete f.commandBuffers;
                }
            }
        }
    }
}

void kirana::viewport::vulkan::Drawer::draw()
{
    const FrameData &frame = getCurrentFrame();
    const uint32_t frameIndex = getCurrentFrameIndex();
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(frame.renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence")
    m_device->current.resetFences(frame.renderFence);

    vk::ResultValue<uint32_t> imgValue = m_swapchain->acquireNextImage(
        constants::VULKAN_FRAME_SYNC_TIMEOUT, frame.presentSemaphore, nullptr);
    if (imgValue.result == vk::Result::eErrorOutOfDateKHR)
    {
        m_device->waitUntilIdle();
        m_onSwapchainOutOfDate();
    }
    else if (imgValue.result != vk::Result::eSuccess &&
             imgValue.result != vk::Result::eSuboptimalKHR)
        VK_HANDLE_RESULT(imgValue.result, "Failed to acquire swapchain image")

    const uint32_t imgIndex = imgValue.value;

    vk::ClearValue clearColor;
    std::array<float, 4> color = {{0.05f, 0.05f, 0.05f, 1.0f}};
    clearColor.setColor(vk::ClearColorValue(color));

    vk::ClearValue clearDepth;
    clearDepth.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));

    frame.commandBuffers->reset();
    frame.commandBuffers->begin();
    frame.commandBuffers->beginRenderPass(
        m_renderPass->current, m_renderPass->framebuffers[imgIndex],
        m_swapchain->imageExtent,
        std::vector<vk::ClearValue>{clearColor, clearDepth});

    if (m_scene)
    {
        MeshPushConstants meshConstants;
        std::string lastMaterial;
        // TODO: Bind Vertex Buffers together and draw them at once.
        size_t meshIndex = 0;
        for (const auto &m : m_scene->getMeshData())
        {
            if (lastMaterial != m.second.material->name)
            {
                frame.commandBuffers->bindPipeline(
                    m.second.material->pipeline->current);
                frame.commandBuffers->bindDescriptorSets(
                    m.second.material->layout->current,
                    {frame.globalDescriptorSet->current,
                     frame.objectDescriptorSet->current},
                    {m_scene->getCameraBufferOffset(frameIndex),
                     m_scene->getWorldDataBufferOffset(frameIndex),
                     m_scene->getObjectBufferOffset(frameIndex)});
                lastMaterial = m.second.material->name;
            }

            frame.commandBuffers->bindVertexBuffer(
                *(m.second.vertexBuffer.buffer), 0);
            frame.commandBuffers->bindIndexBuffer(
                *(m.second.indexBuffer.buffer), 0);

            // TODO: Implement instancing
            for (size_t i = 0; i < m.second.instances.size(); i++)
            {
                frame.commandBuffers->drawIndexed(
                    static_cast<uint32_t>(m.second.indexCount), 1, 0, 0,
                    i + meshIndex);

                // TODO: Find better way to render outline
                if (*m.second.instances[0].selected &&
                    m_scene->shouldRenderOutline())
                {
                    const MaterialData &outline = m_scene->getOutlineMaterial();
                    frame.commandBuffers->bindPipeline(
                        outline.pipeline->current);
                    lastMaterial = outline.name;

                    frame.commandBuffers->drawIndexed(
                        static_cast<uint32_t>(m.second.indexCount), 1, 0, 0,
                        i + meshIndex);
                }
            }
            meshIndex++;
        }
    }

    frame.commandBuffers->endRenderPass();
    frame.commandBuffers->end();

    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsSubmit(frame.presentSemaphore, pipelineFlags,
                             frame.commandBuffers->current[0],
                             frame.renderSemaphore, frame.renderFence);

    vk::Result presentResult = m_device->present(
        frame.renderSemaphore, m_swapchain->current, imgIndex);
    if (presentResult == vk::Result::eErrorOutOfDateKHR)
    {
        m_device->waitUntilIdle();
        m_onSwapchainOutOfDate();
    }
    else if (presentResult != vk::Result::eSuccess &&
             presentResult != vk::Result::eSuboptimalKHR)
        VK_HANDLE_RESULT(presentResult, "Failed to present rendered image")

    m_currentFrameNumber++;
}

void kirana::viewport::vulkan::Drawer::reinitialize(
    const Swapchain *const swapchain, const RenderPass *const renderPass)
{
    m_swapchain = swapchain;
    m_renderPass = renderPass;
}

/*void kirana::viewport::vulkan::Drawer::loadScene(SceneData *scene)
{
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(m_renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence");
    m_scene = scene;
    m_trianglePipeline->rebuild(m_scene->vertexDesc);
}*/
