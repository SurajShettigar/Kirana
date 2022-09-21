#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <memory>

namespace kirana::window
{
class Window;
} // namespace kirana::window

namespace kirana::viewport
{
namespace vulkan
{
class VulkanRenderer;
}
using std::shared_ptr;
using window::Window;
using vulkan::VulkanRenderer;

class Viewport
{
  private:
    shared_ptr<window::Window> m_window;
    VulkanRenderer &m_renderer;
  public:
    Viewport();
    ~Viewport() = default;

    /// Initializes the viewport by binding the window to the renderer (Vulkan).
    void init(const shared_ptr<Window> &window);
    /// Calls the update function of the renderer.
    void update();
    /// Calls the render function of the renderer.
    void render();
    /// Calls the cleanup function of the renderer.
    void clean();
};

} // namespace kirana::viewport
#endif