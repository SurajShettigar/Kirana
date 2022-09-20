#include <iostream>

#include "viewport.hpp"

using std::cerr;
using std::cout;
using std::endl;

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
    m_renderer.clean();
}
