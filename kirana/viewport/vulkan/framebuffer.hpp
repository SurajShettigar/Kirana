#ifndef KIRANA_VIEWPORT_VULKAN_FRAMEBUFFER_HPP
#define KIRANA_VIEWPORT_VULKAN_FRAMEBUFFER_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Texture;

class Framebuffer
{
  private:
    bool m_isInitialized = false;
    vk::Framebuffer m_current;

    const Device *const m_device;
    const RenderPass *const m_renderPass;
    const std::vector<const Texture *> m_colorAttachments;
    const Texture *const m_depthAttachment;
    std::array<uint32_t, 2> m_size;

  public:
    explicit Framebuffer(const Device *device, const RenderPass *renderPass,
                         const std::vector<const Texture *> &colorAttachments,
                         const Texture *depthAttachment,
                         const std::array<uint32_t, 2> &size);
    ~Framebuffer();
    Framebuffer(const Framebuffer &framebuffer) = delete;
    Framebuffer &operator=(const Framebuffer &framebuffer) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Framebuffer &current = m_current;
};
} // namespace kirana::viewport::vulkan

#endif // RENDERPASS_HPP
