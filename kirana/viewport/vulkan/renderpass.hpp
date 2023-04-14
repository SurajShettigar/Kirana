#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class Swapchain;
class Texture;
class Framebuffer;

class RenderPass
{
  private:
    bool m_isInitialized = false;
    std::string m_name;
    const Device *const m_device;
    const Swapchain *const m_swapchain;
    const Texture *m_depthTexture;

    vk::RenderPass m_current = nullptr;
    std::vector<const Framebuffer*> m_framebuffers;

  public:
    explicit RenderPass(std::string name, const Device *device,
                        const Swapchain *swapchain,
                        const Texture *depthTexture);
    ~RenderPass();
    RenderPass(const RenderPass &renderpass) = delete;
    RenderPass &operator=(const RenderPass &renderpass) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
    const vk::RenderPass &current = m_current;
    const std::vector<const Framebuffer*> &framebuffers = m_framebuffers;

    bool resize(const Texture *depthTexture, const std::array<uint32_t, 2> &size);

    [[nodiscard]] std::array<uint32_t, 2> getSurfaceResolution() const;
};
} // namespace kirana::viewport::vulkan

#endif // RENDERPASS_HPP
