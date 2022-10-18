#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP

#include <vulkan/vulkan.hpp>

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

    bool allocateDescriptorSet(const DescriptorSet *&descriptorSet,
                               const DescriptorSetLayout *layout) const;
    bool allocateDescriptorSets(
        const DescriptorSet **descriptorSets,
        const std::vector<DescriptorSetLayout *> &layouts) const;
};
} // namespace kirana::viewport::vulkan

#endif