#include "vulkan_renderer.hpp"

#include <window.hpp>

void kirana::viewport::vulkan::VulkanRenderer::init(
    const std::vector<const char *> &reqInstanceExtensions)
{
    m_instance = new Instance(reqInstanceExtensions);
}

void kirana::viewport::vulkan::VulkanRenderer::initSurface(
    const std::shared_ptr<kirana::window::Window> &window)
{
    VkSurfaceKHR surface;
    if (window->getVulkanWindowSurface(
            static_cast<VkInstance>(m_instance->current), nullptr, &surface) ==
        VK_SUCCESS)
    {
        m_surface = new Surface(m_instance, vk::SurfaceKHR(surface));
    }
}

void kirana::viewport::vulkan::VulkanRenderer::update()
{
}
void kirana::viewport::vulkan::VulkanRenderer::render()
{
}
void kirana::viewport::vulkan::VulkanRenderer::clean()
{
    if (m_surface)
    {
        delete m_surface;
        m_surface = nullptr;
    }
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}