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

void kirana::viewport::Viewport::init(const window::Window *window,
                                      const scene::ViewportScene &scene,
                                      Shading shading)
{
    m_window = window;
    m_prevShading = m_currentShading;
    m_currentShading = shading;
    m_renderer.init(m_window, scene,
                    static_cast<uint16_t>(m_currentShading));
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

void kirana::viewport::Viewport::setShading(Shading shading)
{
    m_prevShading = m_currentShading;
    m_currentShading = shading;
    m_renderer.setShading(static_cast<uint16_t>(m_currentShading));
}

void kirana::viewport::Viewport::toggleWireframe()
{
    if (m_currentShading != Shading::WIREFRAME)
        setShading(Shading::WIREFRAME);
    else
        setShading(m_prevShading);
}

void kirana::viewport::Viewport::togglePBR()
{
    if (m_currentShading != Shading::REALTIME_PBR)
        setShading(Shading::REALTIME_PBR);
    else
        setShading(m_prevShading);
}

/*void kirana::viewport::Viewport::loadScene(const scene::Scene &scene)
{
    m_renderer.loadScene(scene);
}*/
