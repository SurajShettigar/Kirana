#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "instance.hpp"
#include "device.hpp"

#include <memory.h>

namespace kirana::viewport::renderer
{
using std::unique_ptr;
class VulkanRenderer
{
  private:
    unique_ptr<Instance> m_instance;
    unique_ptr<Device> m_device;

  public:
    VulkanRenderer() = default;
    ~VulkanRenderer()
    {
        clean();
    }

    const vk::Instance &getInstance() const;
    void setSurface(const vk::SurfaceKHR &surface);

    void init();
    void update();
    void render();
    void clean();
};
} // namespace kirana::viewport::renderer

#endif