#ifndef KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_HPP
#define KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Texture;
class FrameGraph
{
  public:
    FrameGraph(const Device *device) : m_device{device} {};
    ~FrameGraph() = default;

    FrameGraph(const FrameGraph &frameGraph) = delete;
    FrameGraph &operator=(const FrameGraph &frameGraph) = delete;

    bool addPass(const std::string &passName);
    bool compile();
    void render();

  private:
    bool m_isInitialized = false;
    const Device *const m_device;

    std::vector<const RenderPass *> m_renderPasses;
    std::vector<const Texture *> m_colorAttachments;
    const Texture *m_depthAttachment;
};
} // namespace kirana::viewport::vulkan
#endif
