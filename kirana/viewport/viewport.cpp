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
                                      ShadingPipeline pipeline,
                                      ShadingType type)
{
    m_window = window;
    m_currentPipeline = pipeline;
    m_currentShadingType = type;
    m_renderer.init(m_window, scene,
                    static_cast<vulkan::ShadingPipeline>(m_currentPipeline),
                    static_cast<vulkan::ShadingType>(m_currentShadingType));
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

void kirana::viewport::Viewport::setShading(ShadingPipeline pipeline)
{
    m_currentPipeline = pipeline;
    m_renderer.setShadingPipeline(
        static_cast<vulkan::ShadingPipeline>(m_currentPipeline));
}

void kirana::viewport::Viewport::setShadingType(ShadingType type)
{
    m_currentShadingType = type;
    m_renderer.setShadingType(
        static_cast<vulkan::ShadingType>(m_currentShadingType));
}
