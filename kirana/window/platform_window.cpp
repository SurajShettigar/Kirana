#include "platform_window.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


VkResult kirana::window::PlatformWindow::getVulkanWindowSurface(
    VkInstance instance, const VkAllocationCallbacks *allocator,
    VkSurfaceKHR *surface) const
{
    auto winPtr = reinterpret_cast<HWND>(m_windowPointer);
    auto winInstance =
        reinterpret_cast<HINSTANCE>(GetWindowLongPtr(winPtr, GWLP_HINSTANCE));

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hwnd = winPtr;
    createInfo.hinstance = winInstance;

    return vkCreateWin32SurfaceKHR(instance, &createInfo, allocator, surface);
}

std::vector<const char *> kirana::window::PlatformWindow::
    getReqInstanceExtensionsForVulkan() const
{
    return {"VK_KHR_surface", "VK_KHR_win32_surface"};
}