#ifndef KIRANA_VULKAN_RENDERER_HPP
#define KIRANA_VULKAN_RENDERER_HPP

#include <vector>

namespace kirana::scene
{
class Scene;
}

namespace kirana::window
{
class Window;
}

namespace kirana::viewport::vulkan
{
class Instance;
class Surface;
class Device;
class Allocator;
class Swapchain;
class DepthBuffer;
class RenderPass;
class DescriptorSetLayout;
class DescriptorPool;
class Drawer;
class SceneData;

class VulkanRenderer
{
  private:
    bool m_isMinimized = false;
    const window::Window *m_window = nullptr;

    Allocator *m_allocator = nullptr;
    Instance *m_instance = nullptr;
    Surface *m_surface = nullptr;
    Device *m_device = nullptr;
    Swapchain *m_swapchain = nullptr;
    DepthBuffer *m_depthBuffer = nullptr;
    RenderPass *m_renderpass = nullptr;
    DescriptorPool *m_descriptorPool = nullptr;
    DescriptorSetLayout *m_globalDescSetLayout = nullptr;
    Drawer *m_drawer = nullptr;
    uint32_t m_swapchainOutOfDateListener =
        std::numeric_limits<unsigned int>::max();

    SceneData *m_currentScene = nullptr;

    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

    void rebuildSwapchain();

  public:
    VulkanRenderer(const VulkanRenderer &renderer) = delete;

    static VulkanRenderer &get()
    {
        static VulkanRenderer renderer;
        return renderer;
    }

    /// Initializes vulkan.
    void init(const window::Window *window, scene::Scene &scene,
              uint16_t shadingIndex);
    /// Updates the transforms.
    void update();
    /// Executes vulkan draw calls.
    void render();
    /// Deletes vulkan objects.
    void clean();

    /// Switches the current pipeline to support the required shading.
    void setShading(uint16_t shadingIndex);
    /**
     * Convert Scene object into vulkan SceneData object.
     * @param scene The Scene object to be converted.
     */
    //    void loadScene(const scene::Scene &scene);
};
} // namespace kirana::viewport::vulkan

#endif