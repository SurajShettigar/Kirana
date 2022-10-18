#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include <vector>
#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{

class CommandBuffers
{
  private:
    std::vector<vk::CommandBuffer> m_current;

  public:
    explicit CommandBuffers(std::vector<vk::CommandBuffer> commandBuffers)
        : m_current{std::move(commandBuffers)} {};
    ~CommandBuffers() = default;
    CommandBuffers(const CommandBuffers &buffers) = delete;
    CommandBuffers &operator=(const CommandBuffers &buffers) = delete;

    const std::vector<vk::CommandBuffer> &current = m_current;

    void reset(uint32_t index = 0) const;
    void begin(uint32_t index = 0) const;
    void beginRenderPass(const vk::RenderPass &renderPass,
                         const vk::Framebuffer &framebuffer,
                         vk::Extent2D imageExtent,
                         const std::vector<vk::ClearValue> &clearValues,
                         uint32_t index = 0) const;
    void bindPipeline(const vk::Pipeline &pipeline, uint32_t index = 0) const;
    void bindDescriptorSets(const vk::PipelineLayout &layout,
                            const std::vector<vk::DescriptorSet> &sets,
                            uint32_t index = 0) const;
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
