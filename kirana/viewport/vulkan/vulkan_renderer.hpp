#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include <vector>

namespace kirana::window
{
class Window;
}

namespace kirana::viewport::vulkan
{
class Instance;
class Surface;
class Device;
class Swapchain;

class VulkanRenderer
{
  private:
    Instance *m_instance;
    Surface *m_surface;
    Device *m_device;
    Swapchain *m_swapchain;

    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

  public:
    VulkanRenderer(const VulkanRenderer &renderer) = delete;

    static VulkanRenderer &get()
    {
        static VulkanRenderer renderer;
        return renderer;
    }

    void init(const std::vector<const char *> &reqInstanceExtensions,
              const window::Window *window);
    void update();
    void render();
    void clean();
};
} // namespace kirana::viewport::vulkan

#endif