#include "surface.hpp"
#include "instance.hpp"
#include "vulkan_utils.hpp"

#include <window.hpp>

kirana::viewport::vulkan::Surface::Surface(const Instance *const instance,
                                           const window::Window *const window)
    : m_isInitialized{false}, m_instance{instance}, m_window{window}
{
    VkSurfaceKHR surface;
    if (m_window->getVulkanWindowSurface(
            static_cast<VkInstance>(m_instance->current), nullptr, &surface) ==
        VK_SUCCESS)
    {
        m_current = vk::SurfaceKHR(surface);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Surface created");
        m_isInitialized = true;
    }
}

kirana::viewport::vulkan::Surface::~Surface()
{
    if (m_instance && m_current)
    {
        m_instance->current.destroySurfaceKHR(m_current);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Surface destroyed");
    }
}

std::array<uint32_t, 2> kirana::viewport::vulkan::Surface::getResolution() const
{
    return m_window->resolution;
}