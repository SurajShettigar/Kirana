#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <window.hpp>
#include <renderer/vulkan_renderer.hpp>

#include <memory.h>

namespace kirana::viewport
{
using renderer::VulkanRenderer;
using window::Window;

using std::shared_ptr;

class Viewport
{
  private:
    shared_ptr<Window> m_window;
    VulkanRenderer m_renderer;

  public:
    Viewport() = default;
    ~Viewport()
    {
        clean();
    }

    void init(shared_ptr<Window> window);
    void update();
    void render();
    void clean();
};

} // namespace kirana::viewport
#endif