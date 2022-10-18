#ifndef DESCRIPTOR_SET_HPP
#define DESCRIPTOR_SET_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSet
{
  private:
    vk::DescriptorSet m_current;

    const Device *const m_device;

  public:
    explicit DescriptorSet(const Device *const device,
                           vk::DescriptorSet descriptorSet)
        : m_device{device}, m_current{descriptorSet} {};
    ~DescriptorSet() = default;
    DescriptorSet(const DescriptorSet &descriptorSet) = delete;
    DescriptorSet &operator=(const DescriptorSet &descriptorSet) = delete;

    const vk::DescriptorSet &current = m_current;

    void writeUniformBuffer(const vk::Buffer *buffer, vk::DeviceSize offset,
                            vk::DeviceSize range, uint32_t binding) const;
};
} // namespace kirana::viewport::vulkan


#endif
