#ifndef KIRANA_VIEWPORT_VULKAN_SCENE_RENDER_LAYER_HPP
#define KIRANA_VIEWPORT_VULKAN_SCENE_RENDER_LAYER_HPP

#include "render_layer_base.hpp"


namespace kirana::viewport::vulkan
{
class SceneData;
class DescriptorPool;

class SceneRenderLayer : public RenderLayerBase
{
  public:
    SceneRenderLayer(const Device *device, const Swapchain *swapchain,
                     const Texture *depthTexture, const SceneData *sceneData, const DescriptorPool *descriptorPool);
    ~SceneRenderLayer() override = default;

    SceneRenderLayer(const SceneRenderLayer &layer) = delete;
    SceneRenderLayer &operator=(const SceneRenderLayer &layer) = delete;

    bool render(const CommandBuffers &commandBuffers,
                uint32_t swapchainIndex) override;

  protected:
    const SceneData *const m_sceneData = nullptr;
    const DescriptorPool *const m_descriptorPool = nullptr;
    // TODO: Switch to per-shader pipeline layout using shader reflection.
    const PipelineLayout *m_pipelineLayout = nullptr;
    // TODO: Switch to per-shader descriptor set using shader reflection.
    std::vector<DescriptorSet> m_descSets;
};

} // namespace kirana::viewport::vulkan
#endif