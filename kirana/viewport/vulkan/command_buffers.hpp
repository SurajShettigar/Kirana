#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include <vector>
#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class CommandPool;
class RenderPass;

class CommandBuffers
{
  private:
    bool m_isInitialized = false;
    std::vector<vk::CommandBuffer> m_current;

    const Device *const m_device;
    const CommandPool *const m_commandPool;

  public:
    explicit CommandBuffers(
        const Device *device, const CommandPool *commandPool, size_t count = 1,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
    ~CommandBuffers() = default;
    CommandBuffers(const CommandBuffers &buffers) = delete;
    CommandBuffers &operator=(const CommandBuffers &buffers) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::vector<vk::CommandBuffer> &current = m_current;

    void reset(uint32_t index = 0) const;
    void begin(uint32_t index = 0) const;
    void beginRenderPass(const vk::RenderPass &renderPass,
                         const vk::Framebuffer &framebuffer,
                         vk::Extent2D imageExtent, vk::ClearValue clearColor,
                         uint32_t index = 0) const;
    void bindPipeline(const vk::Pipeline &pipeline, uint32_t index = 0) const;
    void bindVertexBuffer(const vk::Buffer &buffer,
                          const vk::DeviceSize &offset,
                          uint32_t index = 0) const;
    void bindVertexBuffers(const std::vector<vk::Buffer> &buffers,
                           const std::vector<vk::DeviceSize> &offsets,
                           uint32_t index = 0) const;
    // TODO: Temporary solution to push constants.
    void pushConstants(vk::PipelineLayout layout,
                       vk::ShaderStageFlags stageFlags, uint32_t offset,
                       const MeshPushConstants &meshConstants,
                       uint32_t index = 0) const;
    void draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance,
              uint32_t index = 0) const;
    void endRenderPass(uint32_t index = 0) const;
    void end(uint32_t index = 0) const;
};
} // namespace kirana::viewport::vulkan


#endif
