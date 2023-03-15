#ifndef KIRANA_VIEWPORT_VULKAN_RENDER_LAYER_BASE_HPP
#define KIRANA_VIEWPORT_VULKAN_RENDER_LAYER_BASE_HPP

#include "../vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Swapchain;
class Texture;
class RenderPass;
class CommandBuffers;
class RenderLayerBase
{
  public:
    RenderLayerBase(const Device *device, const Swapchain *swapchain,
                    const Texture *depthTexture);
    virtual ~RenderLayerBase();

    RenderLayerBase(const RenderLayerBase &renderLayerBase) = delete;
    RenderLayerBase &operator=(const RenderLayerBase &renderLayerBase) = delete;

    virtual bool render(const CommandBuffers &commandBuffers, uint32_t swapchainIndex) = 0;
  protected:
    bool m_isInitialized = false;
    const Device *const m_device = nullptr;
    const Swapchain *const m_swapchain = nullptr;
    const Texture *const m_depthTexture = nullptr;

    std::array<uint32_t, 2> m_frameBufferSize;
    RenderPass *m_renderPass = nullptr;
    std::vector<vk::Framebuffer> m_frameBuffers;
};
} // namespace kirana::viewport::vulkan

#endif