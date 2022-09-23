#include "command_buffers.hpp"

#include "device.hpp"
#include "command_pool.hpp"
#include "renderpass.hpp"

kirana::viewport::vulkan::CommandBuffers::CommandBuffers(
    const Device *const device, const CommandPool *const commandPool,
    size_t count, vk::CommandBufferLevel level)
    : m_isInitialized{false}, m_device{device}, m_commandPool{commandPool}
{
    vk::CommandBufferAllocateInfo allocateInfo(m_commandPool->current, level,
                                               static_cast<uint32_t>(count));
    try
    {
        m_current = m_device->current.allocateCommandBuffers(allocateInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Command Buffers allocated");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

void kirana::viewport::vulkan::CommandBuffers::reset(uint32_t index) const
{
    m_current[index].reset();
}

void kirana::viewport::vulkan::CommandBuffers::begin(uint32_t index) const
{
    m_current[index].begin(vk::CommandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

void kirana::viewport::vulkan::CommandBuffers::beginRenderPass(
    const vk::RenderPass &renderPass, const vk::Framebuffer &framebuffer,
    vk::Extent2D imageExtent, vk::ClearValue clearColor, uint32_t index) const
{
    m_current[index].beginRenderPass(
        vk::RenderPassBeginInfo(renderPass, framebuffer,
                                vk::Rect2D({0, 0}, imageExtent), clearColor),
        vk::SubpassContents::eInline);
}

void kirana::viewport::vulkan::CommandBuffers::endRenderPass(
    uint32_t index) const
{
    m_current[index].endRenderPass();
}

void kirana::viewport::vulkan::CommandBuffers::end(uint32_t index) const
{
    m_current[index].end();
}