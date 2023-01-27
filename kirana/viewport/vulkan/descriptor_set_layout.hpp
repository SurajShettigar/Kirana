#ifndef DESCRIPTOR_SET_LAYOUT_HPP
#define DESCRIPTOR_SET_LAYOUT_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSetLayout
{
  private:
    bool m_isInitialized = false;
    vk::DescriptorSetLayout m_current;

    const Device *const m_device;

  public:
    explicit DescriptorSetLayout(const Device *device, const std::vector<DescriptorData> &bindingData);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &layout) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &layout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::DescriptorSetLayout &current = m_current;
};

} // namespace kirana::viewport::vulkan


#endif