#include "descriptor_set.hpp"
#include "device.hpp"

void kirana::viewport::vulkan::DescriptorSet::writeUniformBuffer(
    const vk::Buffer *const buffer, vk::DeviceSize offset, vk::DeviceSize range,
    uint32_t binding) const
{
    vk::DescriptorBufferInfo bufferInfo(*buffer, offset, range);
    vk::WriteDescriptorSet write(m_current, binding, 0,
                                 vk::DescriptorType::eUniformBuffer, nullptr,
                                 bufferInfo);
    m_device->current.updateDescriptorSets(write, {});
}