#include "vulkan_renderer.hpp"

#include <window.hpp>

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "swapchain.hpp"

void kirana::viewport::vulkan::VulkanRenderer::init(
    const std::vector<const char *> &reqInstanceExtensions,
    const window::Window *const window)
{
    m_instance = new Instance(reqInstanceExtensions);
    if (m_instance->isInitialized)
    {
        VkSurfaceKHR surface;
        if (window->getVulkanWindowSurface(
                static_cast<VkInstance>(m_instance->current), nullptr,
                &surface) == VK_SUCCESS)
        {
            m_surface = new Surface(m_instance, vk::SurfaceKHR(surface),
                                    window->getWindowResolution());
        }

        if (m_surface->isInitialized)
        {
            m_device = new Device(m_instance, m_surface);
            if (m_device->isInitialized)
            {
                m_swapchain = new Swapchain(m_device, m_surface);
            }
        }
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
    if (m_swapchain)
    {
        delete m_swapchain;
        m_swapchain = nullptr;
    }
    if (m_device)
    {
        delete m_device;
        m_device = nullptr;
    }
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