#ifndef DRAWER_HPP
#define DRAWER_HPP

#include <event.hpp>
#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class DescriptorPool;
class Swapchain;
class RenderPass;
class SceneData;

class Drawer
{
  private:
    bool m_isInitialized = false;
    uint64_t m_currentFrameNumber = 0;

    std::vector<FrameData> m_frames;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const DescriptorPool *const m_descriptorPool;
    const Swapchain *m_swapchain;
    const RenderPass *m_renderPass;

    const SceneData *const m_scene;

    [[nodiscard]] const FrameData &getCurrentFrame() const;
    [[nodiscard]] uint32_t getCurrentFrameIndex() const;

    void rasterizeMeshes(const FrameData &frame,
                         bool drawEditorMeshes);
    void rasterize(const FrameData &frame, uint32_t swapchainImgIndex);
    void raytrace(const FrameData &frame, uint32_t swapchainImgIndex);

  public:
    explicit Drawer(const Device *device, const Allocator *allocator,
                    const DescriptorPool *descriptorPool,
                    const Swapchain *swapchain, const RenderPass *renderPass,
                    const SceneData *scene);
    ~Drawer();
    Drawer(const Drawer &drawer) = delete;
    Drawer &operator=(const Drawer &drawer) = delete;

    const bool &isInitialized = m_isInitialized;

    /// The Vulkan draw calls and synchronization between them are executed
    /// here.
    void draw();

    void reinitialize(const Swapchain *swapchain, const RenderPass *renderPass);
};
} // namespace kirana::viewport::vulkan


#endif // DRAWER_HPP
