#include "surface.hpp"
#include "instance.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::Surface::Surface(
    const Instance *const instance, vk::SurfaceKHR &&surface,
    const std::array<uint32_t, 2> &windowResolution)
    : m_isInitialized{false}, m_instance{instance}, m_windowResolution{
                                                        windowResolution}
{
    m_current = surface;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Surface created");
    m_isInitialized = true;
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