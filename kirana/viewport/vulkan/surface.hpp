#ifndef KIRANA_SURFACE_HPP
#define KIRANA_SURFACE_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::window {
    class Window;
}

namespace kirana::viewport::vulkan
{

class Instance;

class Surface
{
  private:
    bool m_isInitialized = false;
    vk::SurfaceKHR m_current;
    const Instance *const m_instance;
    const window::Window *const m_window;

  public:
    explicit Surface(const Instance *instance, const window::Window *window);
    ~Surface();
    Surface(const Surface &device) = delete;
    Surface &operator=(const Surface &device) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::SurfaceKHR &current = m_current;
    [[nodiscard]]std::array<uint32_t, 2> getResolution() const;
};

} // namespace kirana::viewport::vulkan

#endif // KIRANA_SURFACE_HPP
