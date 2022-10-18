#ifndef PIPELINE_LAYOUT_HPP
#define PIPELINE_LAYOUT_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSetLayout;

class PipelineLayout
{
  private:
    bool m_isInitialized = false;
    vk::PipelineLayout m_current;

    const Device *const m_device;
    const DescriptorSetLayout *const m_globalDescSetLayout;

  public:
    explicit PipelineLayout(const Device *device,
                            const DescriptorSetLayout *globalDescSetLayout);
    ~PipelineLayout();
    PipelineLayout(const PipelineLayout &pipelineLayout) = delete;
    PipelineLayout &operator=(const PipelineLayout &pipelineLayout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::PipelineLayout &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif