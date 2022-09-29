#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <memory>

// Forward declarations
namespace kirana
{
namespace scene
{
class Scene;
}
namespace window
{
class Window;
}
namespace viewport::vulkan
{
class VulkanRenderer;
}
} // namespace kirana

namespace kirana::viewport
{
using std::shared_ptr;
using vulkan::VulkanRenderer;
using window::Window;

class Viewport
{
  private:
    shared_ptr<window::Window> m_window;
    VulkanRenderer &m_renderer;

  public:
    Viewport();
    ~Viewport() = default;

    /// Initializes the viewport by binding the window to the renderer (Vulkan).
    void init(const shared_ptr<Window> &window, const scene::Scene &scene);
    /// Calls the update function of the renderer.
    void update();
    /// Calls the render function of the renderer.
    void render();
    /// Calls the cleanup function of the renderer.
    void clean();

    /**
     * Loads the scene into viewport.
     * @param scene The Scene object to be loaded.
     */
    //    void loadScene(const scene::Scene &scene);
};

} // namespace kirana::viewport
#endif