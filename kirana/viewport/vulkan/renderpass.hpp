#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "vulkan_utils.hpp"
namespace kirana::viewport::vulkan
{
class Device;
class Swapchain;

class RenderPass
{
  private:
    bool m_isInitialized = false;
    vk::RenderPass m_current;
    std::vector<vk::Framebuffer> m_framebuffers;

    const Device *const m_device;
    const Swapchain *const m_swapchain;

  public:
    explicit RenderPass(const Device *device, const Swapchain *swapchain);
    ~RenderPass();
    RenderPass(const RenderPass &renderpass) = delete;
    RenderPass &operator=(const RenderPass &renderpass) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::RenderPass &current = m_current;
};
} // namespace kirana::viewport::vulkan

#endif // RENDERPASS_HPP
