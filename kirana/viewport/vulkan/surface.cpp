#include "surface.hpp"

#include "instance.hpp"

kirana::viewport::vulkan::Surface::Surface(const Instance *const instance,
                                           vk::SurfaceKHR &surface)
    : m_instance{instance}
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                      "Creating Surface...");

    m_current = surface;
}

kirana::viewport::vulkan::Surface::~Surface()
{
    if (m_instance && m_current)
    {
        m_instance->current.destroySurfaceKHR(m_current);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Surface Destroyed");
    }
}