#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
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
    const Swapchain *const m_swapchain;
    const RenderPass *const m_renderPass;

    const SceneData *const m_scene;

    const FrameData &getCurrentFrame() const;
  public:
    explicit Drawer(const Device *device, const Swapchain *swapchain,
                    const RenderPass *renderPass, const SceneData *scene);
    ~Drawer();
    Drawer(const Drawer &drawer) = delete;
    Drawer &operator=(const Drawer &drawer) = delete;

    const bool &isInitialized = m_isInitialized;

    /// The Vulkan draw calls and synchronization between them are executed
    /// here.
    void draw();

    //    void loadScene(SceneData *scene);
};
} // namespace kirana::viewport::vulkan


#endif // DRAWER_HPP
