#ifndef KIRANA_VULKAN_RENDERER_HPP
#define KIRANA_VULKAN_RENDERER_HPP

#include <vector>

namespace kirana::scene
{
class ViewportScene;
}

namespace kirana::window
{
class Window;
}

namespace kirana::viewport
{
enum class Shading;
}

namespace kirana::viewport::vulkan
{
class Instance;
class Surface;
class Device;
class Allocator;
class Swapchain;
class Texture;
class RenderPass;
class DescriptorSetLayout;
class DescriptorPool;
class Drawer;
class SceneData;

class VulkanRenderer
{
  private:
    bool m_isInitialized = false;
    bool m_isMinimized = false;
    const window::Window *m_window = nullptr;

    Allocator *m_allocator = nullptr;
    Instance *m_instance = nullptr;
    Surface *m_surface = nullptr;
    Device *m_device = nullptr;
    Swapchain *m_swapchain = nullptr;
    const Texture *m_depthTexture = nullptr;
    RenderPass *m_renderpass = nullptr;
    DescriptorPool *m_descriptorPool = nullptr;
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
    void init(const window::Window *window, const scene::ViewportScene &scene,
              viewport::Shading shading);
    /// Updates the transforms.
    void update();
    /// Executes vulkan draw calls.
    void render();
    /// Deletes vulkan objects.
    void clean();

    /// Switches the current pipeline to support the required shading.
    void setShading(viewport::Shading shading);
    /**
     * Convert Scene object into vulkan SceneData object.
     * @param scene The Scene object to be converted.
     */
    //    void loadScene(const scene::Scene &scene);
};
} // namespace kirana::viewport::vulkan

#endif