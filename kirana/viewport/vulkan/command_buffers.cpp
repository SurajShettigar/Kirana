#include "command_buffers.hpp"
#include "vulkan_utils.hpp"

void kirana::viewport::vulkan::CommandBuffers::reset(
    vk::CommandBufferResetFlags resetFlags, uint32_t index) const
{
    m_current[index].reset(resetFlags);
}

void kirana::viewport::vulkan::CommandBuffers::begin(
    vk::CommandBufferUsageFlags usageFlags, uint32_t index) const
{
    m_current[index].begin(vk::CommandBufferBeginInfo(usageFlags));
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

void kirana::viewport::vulkan::CommandBuffers::bindDescriptorSets(
    const vk::PipelineLayout &layout,
    const std::vector<vk::DescriptorSet> &sets,
    const std::vector<uint32_t> &dynamicOffsets, uint32_t index) const
{
    m_current[index].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                        layout, 0, sets, dynamicOffsets);
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

void kirana::viewport::vulkan::CommandBuffers::bindIndexBuffer(
    const vk::Buffer &buffer, const vk::DeviceSize &offset,
    vk::IndexType indexType, uint32_t index) const
{
    m_current[index].bindIndexBuffer(buffer, offset, indexType);
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

void kirana::viewport::vulkan::CommandBuffers::drawIndexed(
    uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
    int32_t vertexOffset, uint32_t firstInstance, uint32_t index) const
{
    m_current[index].drawIndexed(indexCount, instanceCount, firstIndex,
                                 vertexOffset, firstInstance);
}

void kirana::viewport::vulkan::CommandBuffers::copyBuffer(
    vk::Buffer srcBuffer, vk::Buffer dstBuffer,
    const std::vector<vk::BufferCopy> &regions, uint32_t index) const
{
    m_current[index].copyBuffer(srcBuffer, dstBuffer, regions);
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


void kirana::viewport::vulkan::CommandBuffers::createMemoryBarrier(
    vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
    vk::DependencyFlags dependencyFlags, const vk::MemoryBarrier &barrier,
    uint32_t index) const
{
    m_current[index].pipelineBarrier(
        srcStageMask, dstStageMask, dependencyFlags, barrier, nullptr, nullptr);
}

void kirana::viewport::vulkan::CommandBuffers::buildAccelerationStructure(
    const vk::AccelerationStructureBuildGeometryInfoKHR &geoInfo,
    const vk::AccelerationStructureBuildRangeInfoKHR *rangeInfo,
    vk::QueryPool &compactionPool, size_t firstCompaction,
    bool addMemoryBarrier, uint32_t index) const
{
    m_current[index].buildAccelerationStructuresKHR(geoInfo, rangeInfo);
    if (addMemoryBarrier)
    {
        createMemoryBarrier(
            vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
            vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
            vk::DependencyFlags(),
            vk::MemoryBarrier(
                vk::AccessFlagBits::eAccelerationStructureWriteKHR,
                vk::AccessFlagBits::eAccelerationStructureReadKHR),
            index);
    }
    if (compactionPool)
    {
        m_current[index].writeAccelerationStructuresPropertiesKHR(
            geoInfo.dstAccelerationStructure,
            vk::QueryType::eAccelerationStructureCompactedSizeKHR,
            compactionPool, firstCompaction);
    }
}