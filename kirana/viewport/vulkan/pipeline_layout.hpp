#ifndef PIPELINE_LAYOUT_HPP
#define PIPELINE_LAYOUT_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;

class PipelineLayout
{
  private:
    bool m_isInitialized = false;
    vk::PipelineLayout m_current;

    const Device *const m_device;

  public:
    explicit PipelineLayout(const Device *device);
    ~PipelineLayout();
    PipelineLayout(const PipelineLayout &pipelineLayout) = delete;
    PipelineLayout &operator=(const PipelineLayout &pipelineLayout) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::PipelineLayout &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif