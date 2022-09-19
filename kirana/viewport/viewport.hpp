#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <vulkan/vulkan_renderer.hpp>
#include <window.hpp>

#include <memory.h>

namespace kirana::viewport
{
using vulkan::VulkanRenderer;
using window::Window;

using std::shared_ptr;

class Viewport
{
  private:
    shared_ptr<Window> m_window;
    VulkanRenderer &m_renderer;

  public:
    Viewport() : m_renderer{VulkanRenderer::get()} {};
    ~Viewport() = default;

    void init(const shared_ptr<Window> &window);
    void update();
    void render();
    void clean();
};

} // namespace kirana::viewport
#endif