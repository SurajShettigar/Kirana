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

kirana::viewport::vulkan::Drawer::Drawer(
    const Device *const device, const Allocator *const allocator,
    const DescriptorPool *const descriptorPool,
    const DescriptorSetLayout *globalDescriptorSetLayout,
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
            bool bufferAllocated = m_allocator->allocateBuffer(
                sizeof(CameraData), vk::BufferUsageFlagBits::eUniformBuffer,
                vma::MemoryUsage::eCpuToGpu, &m_frames[i].cameraBuffer);

            bool setAllocated = m_descriptorPool->allocateDescriptorSet(
                m_frames[i].globalDescriptorSet, globalDescriptorSetLayout);

            if (bufferAllocated && setAllocated)
            {
                m_frames[i].globalDescriptorSet->writeUniformBuffer(
                    m_frames[i].cameraBuffer.buffer.get(), 0,
                    sizeof(CameraData), 0);
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

                m_allocator->free(f.cameraBuffer);
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
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(frame.renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence");
    m_device->current.resetFences(frame.renderFence);

    uint32_t imgIndex = m_swapchain->acquireNextImage(
        constants::VULKAN_FRAME_SYNC_TIMEOUT, frame.presentSemaphore, nullptr);

    vk::ClearValue clearColor;
    std::array<float, 4> color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearColor.setColor(vk::ClearColorValue(color));

    vk::ClearValue clearDepth;
    clearDepth.setDepthStencil(vk::ClearDepthStencilValue(1.0f));

    frame.commandBuffers->reset();
    frame.commandBuffers->begin();
    frame.commandBuffers->beginRenderPass(
        m_renderPass->current, m_renderPass->framebuffers[imgIndex],
        m_swapchain->imageExtent,
        std::vector<vk::ClearValue>{clearColor, clearDepth});

    if (m_scene)
    {
        m_allocator->mapToMemory(frame.cameraBuffer, sizeof(CameraData),
                                 &m_scene->getCameraData());

        MeshPushConstants meshConstants;
        const MaterialData *lastMatData = nullptr;
        // TODO: Bind Vertex Buffers together and draw them at once.
        for (size_t i = 0; i < m_scene->meshes.size(); i++)
        {
            if (lastMatData != m_scene->meshes[i].material)
            {
                frame.commandBuffers->bindPipeline(
                    m_scene->meshes[i].material->pipeline->current);
                frame.commandBuffers->bindDescriptorSets(
                    m_scene->meshes[i].material->layout->current,
                    {frame.globalDescriptorSet->current});
                lastMatData = m_scene->meshes[i].material;
            }
            meshConstants.renderMatrix =
                m_scene->meshes[i].instanceTransforms[0]->getMatrix();

            frame.commandBuffers->pushConstants(
                m_scene->meshes[i].material->layout->current,
                vk::ShaderStageFlagBits::eVertex, 0, meshConstants);

            frame.commandBuffers->bindVertexBuffer(
                *(m_scene->meshes[i].vertexBuffer.buffer), 0);
            frame.commandBuffers->draw(
                static_cast<uint32_t>(m_scene->meshes[i].vertexCount), 1, 0, 0);
        }
    }

    frame.commandBuffers->endRenderPass();
    frame.commandBuffers->end();

    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsQueue.submit(
        vk::SubmitInfo(frame.presentSemaphore, pipelineFlags,
                       frame.commandBuffers->current[0], frame.renderSemaphore),
        frame.renderFence);

    VK_HANDLE_RESULT(
        m_device->presentationQueue.presentKHR(vk::PresentInfoKHR(
            frame.renderSemaphore, m_swapchain->current, imgIndex)),
        "Failed to present rendered image");

    m_currentFrameNumber++;
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
