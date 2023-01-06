#include "shader_binding_table.hpp"

#include "device.hpp"
#include "raytrace_pipeline.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::ShaderBindingTable::ShaderBindingTable(
    const Device *const device, const Allocator *const allocator,
    const RaytracePipeline *const pipeline)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_pipeline{pipeline}
{
    const uint32_t rayGenCount = 1; // There will always be one Ray-Gen shader.
    const uint32_t missCount = 1;
    const uint32_t hitCount = 1;
    const uint32_t callableCount = 0;

    const uint32_t handleCount =
        rayGenCount + missCount + hitCount + callableCount;
    const uint32_t handleSize =
        m_device->raytracingProperties.shaderGroupHandleSize;

    // Shader Binding Table (SBT) needs to be aligned by
    // shaderGroupHandleAlignment.
    const uint32_t handleSizeAligned = m_device->alignSize(
        handleSize, m_device->raytracingProperties.shaderGroupHandleAlignment);

    // SBT buffer region initialization.

    // Ray-Gen is a special case where size of the region should be same as its
    // stride.
    m_rayGenRegion.stride = m_device->alignSize(
        handleSizeAligned,
        m_device->raytracingProperties.shaderGroupBaseAlignment);
    m_rayGenRegion.size = m_rayGenRegion.stride;

    m_missRegion.stride = handleSizeAligned;
    m_missRegion.size = m_device->alignSize(
        missCount * handleSizeAligned,
        m_device->raytracingProperties.shaderGroupBaseAlignment);

    m_hitRegion.stride = handleSizeAligned;
    m_hitRegion.size = m_device->alignSize(
        hitCount * handleSizeAligned,
        m_device->raytracingProperties.shaderGroupBaseAlignment);

    //    m_callableRegion.stride = handleSizeAligned;
    //    m_callableRegion.size = m_device->alignSize(
    //        callableCount * handleSizeAligned,
    //        m_device->raytracingProperties.shaderGroupBaseAlignment);

    // Get Shader Group Handles.
    const size_t dataSize =
        static_cast<size_t>(handleCount) * static_cast<size_t>(handleSize);
    const std::vector<uint8_t> shaderGroupHandles =
        m_device->current.getRayTracingShaderGroupHandlesKHR<uint8_t>(
            m_pipeline->current, 0, handleCount, dataSize);

    // Get Handle data helper lambda function.
    auto getShaderGroupHandle = [&](uint32_t index) {
        return shaderGroupHandles.data() + index * handleSize;
    };

    // Create the SBT Buffer.

    const vk::DeviceSize bufferSize =
        m_rayGenRegion.size + m_missRegion.size +
        m_hitRegion.size /* + m_callableRegion.size*/;

    // Prepare the SBT Buffer data.
    std::vector<uint8_t> bufferData(bufferSize);
    uint8_t *bufferDataPtr = bufferData.data();
    uint32_t handleIndex = 0;

    memcpy(bufferDataPtr, getShaderGroupHandle(handleIndex++), handleSize);
    bufferDataPtr = bufferData.data() + m_rayGenRegion.size;

    for (uint32_t i = 0; i < missCount; i++)
    {
        memcpy(bufferDataPtr, getShaderGroupHandle(handleIndex++), handleSize);
        bufferDataPtr += m_missRegion.stride;
    }
    bufferDataPtr = bufferData.data() + m_rayGenRegion.size + m_missRegion.size;

    for (uint32_t i = 0; i < hitCount; i++)
    {
        memcpy(bufferDataPtr, getShaderGroupHandle(handleIndex++), handleSize);
        bufferDataPtr += m_hitRegion.stride;
    }

    if (!m_allocator->allocateBufferToGPU(
            bufferSize,
            vk::BufferUsageFlagBits::eShaderBindingTableKHR |
                vk::BufferUsageFlagBits::eShaderDeviceAddress,
            &m_buffer, reinterpret_cast<void *>(bufferDataPtr)))
    {

        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to allocate buffer for Shader Binding Table");
        return;
    }

    m_rayGenRegion.deviceAddress = m_device->getBufferAddress(*m_buffer.buffer);
    m_missRegion.deviceAddress =
        m_rayGenRegion.deviceAddress + m_rayGenRegion.size;
    m_hitRegion.deviceAddress = m_missRegion.deviceAddress + m_missRegion.size;

    m_isInitialized = true;

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Binding Table created");
}

kirana::viewport::vulkan::ShaderBindingTable::~ShaderBindingTable()
{
    if (m_buffer.buffer)
        m_allocator->free(m_buffer);

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Binding Table destroyed");
}