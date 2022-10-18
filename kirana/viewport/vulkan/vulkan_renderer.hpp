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
    Allocator *m_allocator;
    Instance *m_instance;
    Surface *m_surface;
    Device *m_device;
    Swapchain *m_swapchain;
    DepthBuffer *m_depthBuffer;
    RenderPass *m_renderpass;
    DescriptorPool *m_descriptorPool;
    DescriptorSetLayout *m_globalDescSetLayout;
    Drawer *m_drawer;

    SceneData *m_currentScene = nullptr;

    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

  public:
    VulkanRenderer(const VulkanRenderer &renderer) = delete;

    static VulkanRenderer &get()
    {
        static VulkanRenderer renderer;
        return renderer;
    }

    /// Initializes vulkan.
    void init(const std::vector<const char *> &reqInstanceExtensions,
              const window::Window *window, const scene::Scene &scene);
    /// Updates the transforms.
    void update();
    /// Executes vulkan draw calls.
    void render();
    /// Deletes vulkan objects.
    void clean();

    /**
     * Convert Scene object into vulkan SceneData object.
     * @param scene The Scene object to be converted.
     */
//    void loadScene(const scene::Scene &scene);
};
} // namespace kirana::viewport::vulkan

#endif