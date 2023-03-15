#ifndef KIRANA_VIEWPORT_VULKAN_PRESENT_RENDER_LAYER_HPP
#define KIRANA_VIEWPORT_VULKAN_PRESENT_RENDER_LAYER_HPP

#include "render_layer_base.hpp"


namespace kirana::viewport::vulkan
{
class PresentRenderLayer : public RenderLayerBase
{
  public:
    PresentRenderLayer(const Device *device, const Swapchain *swapchain,
                     const Texture *depthTexture);
    ~PresentRenderLayer() override = default;

    PresentRenderLayer(const PresentRenderLayer &layer) = delete;
    PresentRenderLayer &operator=(const PresentRenderLayer &layer) = delete;

    bool render(const CommandBuffers &commandBuffers,
                uint32_t swapchainIndex) override;
};

} // namespace kirana::viewport::vulkan
#endif