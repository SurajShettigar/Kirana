#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"

namespace kirana::window
{
class Window;
}

namespace kirana::viewport::vulkan
{
class VulkanRenderer
{
  private:
    Instance *m_instance;
    Surface *m_surface;
    Device *m_device;

    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

  public:
    VulkanRenderer(const VulkanRenderer &renderer) = delete;

    static VulkanRenderer &get()
    {
        static VulkanRenderer renderer;
        return renderer;
    }

    void init(const std::vector<const char *> &reqInstanceExtensions);
    void initSurface(const std::shared_ptr<kirana::window::Window> &window);
    void update();
    void render();
    void clean();
};
} // namespace kirana::viewport::vulkan

#endif