#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Pipeline
{
  private:
    bool m_isInitialized = false;
    vk::Pipeline m_current;

    const Device *const m_device;
    const RenderPass *const m_renderPass;
  public:
    explicit Pipeline(const Device *device, const RenderPass *renderPass);
    ~Pipeline();
    Pipeline(const Pipeline &pipeline) = delete;
    Pipeline &operator=(const Pipeline &pipeline) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Pipeline &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif