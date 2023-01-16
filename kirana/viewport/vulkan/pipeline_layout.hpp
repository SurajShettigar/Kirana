#ifndef PIPELINE_LAYOUT_HPP
#define PIPELINE_LAYOUT_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSetLayout;
class PushConstantBase;

class PipelineLayout
{
  private:
    bool m_isInitialized = false;
    vk::PipelineLayout m_current;

    const Device *const m_device;

  public:
    explicit PipelineLayout(
        const Device *device,
        const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
        const std::vector<const PushConstantBase *> &pushConstants);
    ~PipelineLayout();
    PipelineLayout(const PipelineLayout &pipelineLayout) = delete;
    PipelineLayout &operator=(const PipelineLayout &pipelineLayout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::PipelineLayout &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif