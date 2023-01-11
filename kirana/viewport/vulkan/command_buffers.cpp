#include "command_buffers.hpp"
#include "vulkan_utils.hpp"

#include "shader_binding_table.hpp"
#include "texture.hpp"


void kirana::viewport::vulkan::CommandBuffers::getAccessMasksFromImageLayouts(
    const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout,
    vk::AccessFlags *srcAccessMask, vk::AccessFlags *dstAccessMask)
{
    // Taken from Sascha Williams Vulkan repo example code.
    // Link:
    // https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanTools.cpp
    switch (oldLayout)
    {
    case vk::ImageLayout::eUndefined:
        *srcAccessMask = vk::AccessFlagBits::eNone;
        break;
    case vk::ImageLayout::ePreinitialized:
        *srcAccessMask = vk::AccessFlagBits::eHostWrite;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        *srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        *srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        *srcAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
    case vk::ImageLayout::eTransferDstOptimal:
        *srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        *srcAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
    default:
        break;
    }

    switch (newLayout)
    {
    case vk::ImageLayout::eTransferDstOptimal:
        *dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        *dstAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        *dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        *dstAccessMask =
            *dstAccessMask | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        if (*srcAccessMask == vk::AccessFlagBits::eNone)
            *srcAccessMask = vk::AccessFlagBits::eHostWrite |
                             vk::AccessFlagBits::eTransferWrite;
        *dstAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
    default:
        break;
    }
}

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

void kirana::viewport::vulkan::CommandBuffers::createMemoryBarrier(
    vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
    vk::DependencyFlags dependencyFlags, vk::AccessFlags srcAccessMask,
    vk::AccessFlagBits dstAccessMask, uint32_t index) const
{
    m_current[index].pipelineBarrier(
        srcStageMask, dstStageMask, dependencyFlags,
        vk::MemoryBarrier(srcAccessMask, dstAccessMask), nullptr, nullptr);
}

void kirana::viewport::vulkan::CommandBuffers::createImageMemoryBarrier(
    vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
    vk::DependencyFlags dependencyFlags, vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout, vk::Image image,
    vk::ImageSubresourceRange subresourceRange, uint32_t index) const
{
    vk::ImageMemoryBarrier imgBarrier{{}, {}, oldLayout, newLayout,
                                      {}, {}, image,     subresourceRange};
    getAccessMasksFromImageLayouts(oldLayout, newLayout,
                                   &imgBarrier.srcAccessMask,
                                   &imgBarrier.dstAccessMask);
    m_current[index].pipelineBarrier(srcStageMask, dstStageMask,
                                     dependencyFlags, nullptr, nullptr,
                                     imgBarrier);
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

void kirana::viewport::vulkan::CommandBuffers::setViewportScissor(
    const vk::Viewport &viewport, const vk::Rect2D &scissor,
    uint32_t index) const
{
    m_current[index].setViewport(0, viewport);
    m_current[index].setScissor(0, scissor);
}

void kirana::viewport::vulkan::CommandBuffers::bindPipeline(
    const vk::Pipeline &pipeline, vk::PipelineBindPoint bindPoint,
    uint32_t index) const
{
    m_current[index].bindPipeline(bindPoint, pipeline);
}

void kirana::viewport::vulkan::CommandBuffers::bindDescriptorSets(
    const vk::PipelineLayout &layout,
    const std::vector<vk::DescriptorSet> &sets,
    const std::vector<uint32_t> &dynamicOffsets,
    vk::PipelineBindPoint bindPoint, uint32_t index) const
{
    m_current[index].bindDescriptorSets(bindPoint, layout, 0, sets,
                                        dynamicOffsets);
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

void kirana::viewport::vulkan::CommandBuffers::traceRays(
    const ShaderBindingTable &sbt, const std::array<uint32_t, 3> &size,
    uint32_t index) const
{
    m_current[index].traceRaysKHR(sbt.rayGenRegion, sbt.missRegion,
                                  sbt.hitRegion, {}, size[0],
                                  size[1], size[2]);
}

void kirana::viewport::vulkan::CommandBuffers::copyBuffer(
    vk::Buffer srcBuffer, vk::Buffer dstBuffer,
    const std::vector<vk::BufferCopy> &regions, uint32_t index) const
{
    m_current[index].copyBuffer(srcBuffer, dstBuffer, regions);
}

void kirana::viewport::vulkan::CommandBuffers::copyImage(
    const Texture &srcImage, const Texture &dstImage,
    const std::array<uint32_t, 3> &copyExtent,
    const std::array<int32_t, 3> &srcImageOffset,
    const std::array<int32_t, 3> &dstImageOffset, uint32_t index) const
{
    const vk::ImageSubresourceRange &srcSubRR =
        srcImage.getImageSubresourceRange();
    const vk::ImageSubresourceRange &dstSubRR =
        dstImage.getImageSubresourceRange();

    // Transition image layouts from its original layout to transfer layouts.
    createImageMemoryBarrier(vk::PipelineStageFlagBits::eAllCommands,
                             vk::PipelineStageFlagBits::eAllCommands, {},
                             vk::ImageLayout::eUndefined,
                             vk::ImageLayout::eTransferDstOptimal,
                             dstImage.getImage(), dstSubRR);
    createImageMemoryBarrier(vk::PipelineStageFlagBits::eAllCommands,
                             vk::PipelineStageFlagBits::eAllCommands, {},
                             srcImage.getProperties().layout,
                             vk::ImageLayout::eTransferSrcOptimal,
                             srcImage.getImage(), srcSubRR);

    const vk::ImageCopy copy{
        {srcSubRR.aspectMask, srcSubRR.baseMipLevel, srcSubRR.baseArrayLayer,
         srcSubRR.layerCount},
        {srcImageOffset[0], srcImageOffset[1], srcImageOffset[2]},
        {dstSubRR.aspectMask, dstSubRR.baseMipLevel, dstSubRR.baseArrayLayer,
         dstSubRR.layerCount},
        {dstImageOffset[0], dstImageOffset[1], dstImageOffset[2]},
        {copyExtent[0], copyExtent[1], copyExtent[2]}};

    m_current[index].copyImage(
        srcImage.getImage(), vk::ImageLayout::eTransferSrcOptimal,
        dstImage.getImage(), vk::ImageLayout::eTransferDstOptimal, copy);

    // Transition image layouts back to its original layouts.
    createImageMemoryBarrier(vk::PipelineStageFlagBits::eAllCommands,
                             vk::PipelineStageFlagBits::eAllCommands, {},
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::ImageLayout::ePresentSrcKHR,
                             dstImage.getImage(), dstSubRR);
    createImageMemoryBarrier(vk::PipelineStageFlagBits::eAllCommands,
                             vk::PipelineStageFlagBits::eAllCommands, {},
                             vk::ImageLayout::eTransferSrcOptimal,
                             srcImage.getProperties().layout,
                             srcImage.getImage(), srcSubRR);
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

void kirana::viewport::vulkan::CommandBuffers::buildAccelerationStructure(
    const vk::AccelerationStructureBuildGeometryInfoKHR &geoInfo,
    const vk::AccelerationStructureBuildRangeInfoKHR *rangeInfo,
    vk::QueryPool &compactionPool, uint32_t firstCompaction,
    bool addMemoryBarrier, uint32_t index) const
{
    m_current[index].buildAccelerationStructuresKHR(geoInfo, rangeInfo);
    if (addMemoryBarrier)
    {
        createMemoryBarrier(
            vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
            vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
            vk::DependencyFlags(),
            vk::AccessFlagBits::eAccelerationStructureWriteKHR,
            vk::AccessFlagBits::eAccelerationStructureReadKHR, index);
    }
    if (compactionPool)
    {
        m_current[index].writeAccelerationStructuresPropertiesKHR(
            geoInfo.dstAccelerationStructure,
            vk::QueryType::eAccelerationStructureCompactedSizeKHR,
            compactionPool, firstCompaction);
    }
}