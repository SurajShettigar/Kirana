#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSet;
class DescriptorSetLayout;
class DescriptorPool
{
  private:
    bool m_isInitialized = false;
    vk::DescriptorPool m_current;

    const Device *const m_device;

  public:
    explicit DescriptorPool(const Device *device);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &pool) = delete;
    DescriptorPool &operator=(const DescriptorPool &pool) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::DescriptorPool &current = m_current;

    bool allocateDescriptorSet(const DescriptorSetLayout *layout,
                               DescriptorSet *set) const;
    bool allocateDescriptorSets(const std::vector<const DescriptorSetLayout *> &layouts,
                                std::vector<DescriptorSet> *sets) const;

    void writeDescriptorSet(const DescriptorSet &set) const;
};
} // namespace kirana::viewport::vulkan

#endif