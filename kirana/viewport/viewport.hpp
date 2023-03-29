#ifndef KIRANA_VIEWPORT_VIEWPORT_HPP
#define KIRANA_VIEWPORT_VIEWPORT_HPP

#include "viewport_types.hpp"

#include <memory>
#include <array>

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
using vulkan::VulkanRenderer;
class Viewport
{
  private:
    const window::Window *m_window;
    VulkanRenderer &m_renderer;
    ShadingPipeline m_currentPipeline = ShadingPipeline::REALTIME;
    ShadingType m_currentShadingType = ShadingType::BASIC;
  public:
    Viewport();
    ~Viewport() = default;

    /// Initializes the viewport by binding the window to the renderer (Vulkan).
    void init(const window::Window *window, const scene::Scene &editorScene,
              ShadingPipeline pipeline = ShadingPipeline::REALTIME,
              ShadingType type = ShadingType::BASIC);
    /// Calls the update function of the renderer.
    void update();
    /// Calls the render function of the renderer.
    void render();
    /// Calls the cleanup function of the renderer.
    void clean();

    void loadScene(const scene::Scene &scene);

    /// Changes the current viewport shading pipeline.
    void setShading(ShadingPipeline pipeline);
    /// Changes the current viewport shading type.
    void setShadingType(ShadingType type);
};

} // namespace kirana::viewport
#endif