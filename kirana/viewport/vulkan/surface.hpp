#ifndef KIRANA_SURFACE_HPP
#define KIRANA_SURFACE_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{

class Instance;

class Surface
{
  private:
    bool m_isInitialized = false;
    vk::SurfaceKHR m_current;
    const Instance *const m_instance;
    const std::array<int, 2> m_windowResolution;

  public:
    explicit Surface(const Instance *instance, vk::SurfaceKHR &&surface,
                     const std::array<int, 2> windowResolution);
    ~Surface();
    Surface(const Surface &device) = delete;
    Surface &operator=(const Surface &device) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::SurfaceKHR &current = m_current;
    const std::array<int, 2>& windowResolution = m_windowResolution;

};

} // namespace kirana::viewport::vulkan

#endif // KIRANA_SURFACE_HPP
