#include <iostream>

#include "viewport.hpp"

using std::cerr;
using std::cout;
using std::endl;

void kirana::viewport::Viewport::init(shared_ptr<Window> window)
{
    m_window = window;

    m_renderer.init();

    // VkSurfaceKHR surface;
    // m_window->getVulkanWindowSurface(
    //     static_cast<VkInstance>(m_renderer.instance), nullptr, &surface);
    // m_renderer.surface = vk::SurfaceKHR(surface);
}

void kirana::viewport::Viewport::update()
{
    m_renderer.update();
}

void kirana::viewport::Viewport::render()
{
    m_renderer.render();
}

void kirana::viewport::Viewport::clean()
{
    m_renderer.clean();
}
