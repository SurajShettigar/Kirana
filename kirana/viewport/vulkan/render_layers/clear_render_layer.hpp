#ifndef KIRANA_VIEWPORT_VULKAN_CLEAR_RENDER_LAYER_HPP
#define KIRANA_VIEWPORT_VULKAN_CLEAR_RENDER_LAYER_HPP

#include "render_layer_base.hpp"


namespace kirana::viewport::vulkan
{
class ClearRenderLayer : public RenderLayerBase
{
  public:
    ClearRenderLayer(const Device *device, const Swapchain *swapchain,
                     const Texture *depthTexture,
                     const math::Vector4 &clearColor = math::Vector4::ZERO,
                     const math::Vector2 &clearDepthStencil = math::Vector2{
                         1.0f, 0.0f});
    ~ClearRenderLayer() override = default;

    ClearRenderLayer(const ClearRenderLayer &layer) = delete;
    ClearRenderLayer &operator=(const ClearRenderLayer &layer) = delete;

    bool render(const CommandBuffers &commandBuffers,
                uint32_t swapchainIndex) override;

  protected:
    math::Vector4 m_clearColor;
    math::Vector2 m_clearDepthStencil;
};

} // namespace kirana::viewport::vulkan
#endif