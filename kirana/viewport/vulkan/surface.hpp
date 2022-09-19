#ifndef KIRANA_SURFACE_HPP
#define KIRANA_SURFACE_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{

class Instance;

class Surface
{
  private:
    vk::SurfaceKHR m_current;
    const Instance *const m_instance;

  public:
    Surface(const Instance *instance, vk::SurfaceKHR &surface);
    ~Surface();

    Surface(const Surface &device) = delete;
    Surface &operator=(const Surface &device) = delete;

    const vk::SurfaceKHR &current = m_current;
};

} // namespace kirana::viewport::vulkan

#endif // KIRANA_SURFACE_HPP
