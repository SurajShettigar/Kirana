#include "command_buffers.hpp"

#include "device.hpp"
#include "command_pool.hpp"
#include "vulkan_utils.hpp"


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
    vk::Extent2D imageExtent, const std::vector<vk::ClearValue> &clearValues,
    uint32_t index) const
{
    m_current[index].beginRenderPass(
        vk::RenderPassBeginInfo(renderPass, framebuffer,
                                vk::Rect2D({0, 0}, imageExtent), clearValues),
        vk::SubpassContents::eInline);
}

void kirana::viewport::vulkan::CommandBuffers::bindPipeline(
    const vk::Pipeline &pipeline, uint32_t index) const
{
    m_current[index].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
}


void kirana::viewport::vulkan::CommandBuffers::bindVertexBuffer(
    const vk::Buffer &buffer, const vk::DeviceSize &offset,
    uint32_t index) const
{
    m_current[index].bindVertexBuffers(0, buffer, offset);
}

void kirana::viewport::vulkan::CommandBuffers::bindVertexBuffers(
    const std::vector<vk::Buffer> &buffers,
    const std::vector<vk::DeviceSize> &offsets, uint32_t index) const
{
    m_current[index].bindVertexBuffers(0, buffers, offsets);
}

// TODO: Temporary solution to push constants.
void kirana::viewport::vulkan::CommandBuffers::pushConstants(
    vk::PipelineLayout layout, vk::ShaderStageFlags stageFlags, uint32_t offset,
    const MeshPushConstants &meshConstants, uint32_t index) const
{
    m_current[index].pushConstants<MeshPushConstants>(layout, stageFlags,
                                                      offset, meshConstants);
}

void kirana::viewport::vulkan::CommandBuffers::draw(uint32_t vertexCount,
                                                    uint32_t instanceCount,
                                                    uint32_t firstVertex,
                                                    uint32_t firstInstance,
                                                    uint32_t index) const
{
    m_current[index].draw(vertexCount, instanceCount, firstVertex,
                          firstInstance);
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