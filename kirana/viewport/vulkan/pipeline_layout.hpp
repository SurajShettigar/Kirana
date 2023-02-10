#ifndef PIPELINE_LAYOUT_HPP
#define PIPELINE_LAYOUT_HPP

#include "vulkan_types.hpp"

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
    std::vector<const DescriptorSetLayout *> m_descLayouts;
    std::vector<const PushConstantBase *> m_pushConstants;

  public:
    explicit PipelineLayout(
        const Device *device,
        std::vector<const DescriptorSetLayout *> descriptorSetLayouts,
        std::vector<const PushConstantBase *> pushConstants);
    ~PipelineLayout();
    PipelineLayout(const PipelineLayout &pipelineLayout) = delete;
    PipelineLayout &operator=(const PipelineLayout &pipelineLayout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::PipelineLayout &current = m_current;

    static bool getDefaultPipelineLayout(
        const Device *device, vulkan::ShadingPipeline shadingPipeline,
        const PipelineLayout *&layout);

    [[nodiscard]] inline std::vector<const DescriptorSetLayout *>
    getDescriptorSetLayouts() const
    {
        return m_descLayouts;
    }
};
} // namespace kirana::viewport::vulkan
#endif