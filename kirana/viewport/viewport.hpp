#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "viewport_types.hpp"

#include <memory>
#include <array>

// Forward declarations
namespace kirana
{
namespace scene
{
class ViewportScene;
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
using vulkan::VulkanRenderer;
class Viewport
{
  private:
    const window::Window *m_window;
    VulkanRenderer &m_renderer;
    Shading m_prevShading = Shading::BASIC;
    Shading m_currentShading = Shading::BASIC;

  public:
    Viewport();
    ~Viewport() = default;

    /// Initializes the viewport by binding the window to the renderer (Vulkan).
    void init(const window::Window *window, const scene::ViewportScene &scene,
              Shading shading = Shading::BASIC);
    /// Calls the update function of the renderer.
    void update();
    /// Calls the render function of the renderer.
    void render();
    /// Calls the cleanup function of the renderer.
    void clean();

    /// Changes the current viewport shading.
    void setShading(Shading shading);
    /// Toggles wireframe shading.
    void toggleWireframe();
    /// Toggles realtime PBR shading.
    void togglePBR();
    /// Toggles raytraced PBR shading.
    void toggleRaytracePBR();
    /**
     * Loads the scene into viewport.
     * @param scene The Scene object to be loaded.
     */
    //    void loadScene(const scene::Scene &scene);
};

} // namespace kirana::viewport
#endif