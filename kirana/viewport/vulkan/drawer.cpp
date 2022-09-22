#include "drawer.hpp"

#include "command_pool.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"

// #include <math.h>

kirana::viewport::vulkan::Drawer::Drawer(const Device *const device,
                                         const Swapchain *const swapchain,
                                         const RenderPass *const renderPass)
    : m_isInitialized{false}, m_currentFrameNumber{0}, m_device{device},
      m_swapchain{swapchain}, m_renderPass{renderPass}
{
    m_commandPool =
        new CommandPool(m_device, m_device->queueFamilyIndices.graphics);
    m_mainCommandBuffers = new CommandBuffers(m_device, m_commandPool);
    try
    {
        m_renderFence = m_device->current.createFence(
            vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        m_renderSemaphore =
            m_device->current.createSemaphore(vk::SemaphoreCreateInfo());
        m_presentSemaphore =
            m_device->current.createSemaphore(vk::SemaphoreCreateInfo());

        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Synchronization structures initialized");
        m_isInitialized = true;
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::Drawer::~Drawer()
{
    if (m_device)
    {
        if (m_renderFence)
        {
            if (m_device->current.waitForFences(
                    m_renderFence, true,
                    constants::VULKAN_FRAME_SYNC_TIMEOUT) !=
                vk::Result::eSuccess)
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error,
                                  "Failed to wait for Render Fence");
            m_device->current.destroyFence(m_renderFence);
        }
        if (m_renderSemaphore)
            m_device->current.destroySemaphore(m_renderSemaphore);
        if (m_presentSemaphore)
            m_device->current.destroySemaphore(m_presentSemaphore);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Synchronization structures destroyed");

        if (m_commandPool)
        {
            delete m_commandPool;
            m_commandPool = nullptr;
        }
        if (m_mainCommandBuffers)
        {
            delete m_mainCommandBuffers;
            m_mainCommandBuffers = nullptr;
        }
    }
}

void kirana::viewport::vulkan::Drawer::draw()
{
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(m_renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence");
    m_device->current.resetFences(m_renderFence);

    uint32_t imgIndex = m_swapchain->acquireNextImage(
        constants::VULKAN_FRAME_SYNC_TIMEOUT, m_presentSemaphore, nullptr);

    vk::ClearValue clearValue;
    std::array<float, 4> color = {
        {abs(sin(static_cast<float>(m_currentFrameNumber) / 120.f)),
         abs(sin(static_cast<float>(m_currentFrameNumber) / 240.f)),
         abs(sin(static_cast<float>(m_currentFrameNumber) / 360.f)), 1.0f}};
    clearValue.setColor(vk::ClearColorValue(color));

    m_mainCommandBuffers->reset();
    m_mainCommandBuffers->begin();
    m_mainCommandBuffers->beginRenderPass(m_renderPass->current,
                                          m_renderPass->framebuffers[imgIndex],
                                          m_swapchain->imageExtent, clearValue);
    m_mainCommandBuffers->endRenderPass();
    m_mainCommandBuffers->end();

    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsQueue.submit(
        vk::SubmitInfo(m_presentSemaphore, pipelineFlags,
                       m_mainCommandBuffers->current[0], m_renderSemaphore),
        m_renderFence);

    VK_HANDLE_RESULT(m_device->presentationQueue.presentKHR(vk::PresentInfoKHR(
                         m_renderSemaphore, m_swapchain->current, imgIndex)),
                     "Failed to present rendered image");

    m_currentFrameNumber++;
}
