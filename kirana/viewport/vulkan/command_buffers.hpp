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

    void reset(vk::CommandBufferResetFlags resetFlags =
                   vk::CommandBufferResetFlagBits::eReleaseResources,
               uint32_t index = 0) const;
    void begin(vk::CommandBufferUsageFlags usageFlags =
                   vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
               uint32_t index = 0) const;
    void beginRenderPass(const vk::RenderPass &renderPass,
                         const vk::Framebuffer &framebuffer,
                         vk::Extent2D imageExtent,
                         const std::vector<vk::ClearValue> &clearValues,
                         uint32_t index = 0) const;
    void bindPipeline(const vk::Pipeline &pipeline, uint32_t index = 0) const;
    void bindDescriptorSets(const vk::PipelineLayout &layout,
                            const std::vector<vk::DescriptorSet> &sets,
                            const std::vector<uint32_t> &dynamicOffsets,
                            uint32_t index = 0) const;
    void bindVertexBuffer(const vk::Buffer &buffer,
                          const vk::DeviceSize &offset,
                          uint32_t index = 0) const;
    void bindVertexBuffers(const std::vector<vk::Buffer> &buffers,
                           const std::vector<vk::DeviceSize> &offsets,
                           uint32_t index = 0) const;
    void bindIndexBuffer(const vk::Buffer &buffer, const vk::DeviceSize &offset,
                         vk::IndexType indexType = vk::IndexType::eUint32,
                         uint32_t index = 0) const;
    // TODO: Temporary solution to push constants.
    void pushConstants(vk::PipelineLayout layout,
                       vk::ShaderStageFlags stageFlags, uint32_t offset,
                       const MeshPushConstants &meshConstants,
                       uint32_t index = 0) const;
    void draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance,
              uint32_t index = 0) const;
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance, uint32_t index = 0) const;
    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer,
                    const std::vector<vk::BufferCopy> &regions,
                    uint32_t index = 0) const;
    void endRenderPass(uint32_t index = 0) const;
    void end(uint32_t index = 0) const;

    void createMemoryBarrier(vk::PipelineStageFlags srcStageMask,
                             vk::PipelineStageFlags dstStageMask,
                             vk::DependencyFlags dependencyFlags,
                             const vk::MemoryBarrier &barrier,
                             uint32_t index = 0) const;

    // Raytracing functions
    void buildAccelerationStructure(
        const vk::AccelerationStructureBuildGeometryInfoKHR &geoInfo,
        const vk::AccelerationStructureBuildRangeInfoKHR *rangeInfo,
        vk::QueryPool &compactionPool, size_t firstCompaction,
        bool addMemoryBarrier = true, uint32_t index = 0) const;
};
} // namespace kirana::viewport::vulkan


#endif
