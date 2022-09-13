#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "vulkan_utils.hpp"
namespace kirana::viewport::renderer
{
class VulkanRenderer
{
  private:
    vk::Instance m_instance;
    vk::SurfaceKHR m_surface;

  public:
    VulkanRenderer() = default;
    ~VulkanRenderer()
    {
        clean();
    }

    const vk::Instance &instance = m_instance;
    vk::SurfaceKHR &surface = m_surface;

    void init();
    void update();
    void render();
    void clean();
};
} // namespace kirana::viewport::renderer

#endif