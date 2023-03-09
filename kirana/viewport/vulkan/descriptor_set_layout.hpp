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
    std::vector<DescriptorBindingInfo> m_bindings;
    bool m_hasDynamicDescriptorBindings = false;
    vk::DescriptorSetLayout m_current;

    const Device *const m_device;

  public:
    explicit DescriptorSetLayout(
        const Device *device,
        const std::vector<DescriptorBindingInfo> &bindings,
        bool dynamicDescriptors = false);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &layout) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &layout) = delete;

    const bool &isInitialized = m_isInitialized;
    const bool &hasDynamicDescriptorBindings = m_hasDynamicDescriptorBindings;
    const vk::DescriptorSetLayout &current = m_current;

    [[nodiscard]] inline const std::vector<DescriptorBindingInfo> &getBindings()
        const
    {
        return m_bindings;
    }
    [[nodiscard]] bool containsBinding(const DescriptorBindingInfo &bindingInfo) const;


    static DescriptorBindingInfo getBindingInfoForData(
        DescriptorBindingDataType dataType,
        vulkan::ShadingPipeline shadingPipeline);
    static bool getDefaultDescriptorLayout(const Device *device,
                                           DescriptorLayoutType layoutType,
                                           vulkan::ShadingPipeline pipeline,
                                           const DescriptorSetLayout *&layout);
};

} // namespace kirana::viewport::vulkan


#endif