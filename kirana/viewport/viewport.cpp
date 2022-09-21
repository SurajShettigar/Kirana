

#include "viewport.hpp"
#include <iostream>
#include <vulkan/vulkan_renderer.hpp>
#include <window.hpp>

using std::cerr;
using std::cout;
using std::endl;


kirana::viewport::Viewport::Viewport()
    : m_renderer{kirana::viewport::vulkan::VulkanRenderer::get()}
{
}

void kirana::viewport::Viewport::init(const shared_ptr<Window> &window)
{
    m_window = window;
    m_renderer.init(m_window->getReqInstanceExtensionsForVulkan(),
                    m_window.get());
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
