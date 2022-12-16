#include "descriptor_set.hpp"
#include "device.hpp"

void kirana::viewport::vulkan::DescriptorSet::writeBuffer(
    const vk::DescriptorBufferInfo &bufferInfo, vk::DescriptorType type,
    uint32_t binding) const
{
    vk::WriteDescriptorSet write(m_current, binding, 0, type, nullptr,
                                 bufferInfo);
    m_device->current.updateDescriptorSets(write, {});
}


void kirana::viewport::vulkan::DescriptorSet::writeAccelerationStructure(
    const vk::AccelerationStructureKHR &accelStruct, uint32_t binding) const
{
    const vk::WriteDescriptorSetAccelerationStructureKHR writeInfo{accelStruct};
    vk::WriteDescriptorSet write(m_current, binding, 0,
                                 vk::DescriptorType::eAccelerationStructureKHR,
                                 nullptr);
    write.pNext = &writeInfo;
    m_device->current.updateDescriptorSets(write, {});
}