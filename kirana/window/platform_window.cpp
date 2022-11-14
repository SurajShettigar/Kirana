#include "platform_window.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <iostream>

WNDPROC prevWindowProc;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_SPACE:
            std::cout << "Space Down" << std::endl;
        }
        break;
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_SPACE:
            std::cout << "Space Up" << std::endl;
        }
        break;
    }
    return CallWindowProc(prevWindowProc, hwnd, uMsg, wParam, lParam);
}

kirana::window::PlatformWindow::PlatformWindow(int windowPointer, string name,
                                               bool fullscreen, bool resizable,
                                               int width, int height)
    : Window(std::move(name), fullscreen, resizable, width, height),
      m_windowPointer{windowPointer},
      m_hwndWindowPointer{reinterpret_cast<HWND>(m_windowPointer)}
{
    prevWindowProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(m_hwndWindowPointer, GWLP_WNDPROC,
                         reinterpret_cast<LONG_PTR>(&WindowProc)));

    SetFocus(m_hwndWindowPointer);

    m_resolution[0] = static_cast<uint32_t>(width);
    m_resolution[1] = static_cast<uint32_t>(height);
};

VkResult kirana::window::PlatformWindow::getVulkanWindowSurface(
    VkInstance instance, const VkAllocationCallbacks *allocator,
    VkSurfaceKHR *surface) const
{
    auto winInstance = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtr(m_hwndWindowPointer, GWLP_HINSTANCE));

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hwnd = m_hwndWindowPointer;
    createInfo.hinstance = winInstance;

    return vkCreateWin32SurfaceKHR(instance, &createInfo, allocator, surface);
}

std::vector<const char *> kirana::window::PlatformWindow::
    getReqInstanceExtensionsForVulkan() const
{
    return {"VK_KHR_surface", "VK_KHR_win32_surface"};
}