#ifndef DESCRIPTOR_SET_LAYOUT_HPP
#define DESCRIPTOR_SET_LAYOUT_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSetLayout
{
  public:
    enum class LayoutType
    {
        GLOBAL = 0,
        OBJECT = 1,
        RAYTRACE = 2,
    };

  private:
    bool m_isInitialized = false;
    vk::DescriptorSetLayout m_current;

    const Device *const m_device;

  public:
    explicit DescriptorSetLayout(const Device *device,
                                 LayoutType layoutType = LayoutType::GLOBAL);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &layout) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &layout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::DescriptorSetLayout &current = m_current;
};

} // namespace kirana::viewport::vulkan


#endif