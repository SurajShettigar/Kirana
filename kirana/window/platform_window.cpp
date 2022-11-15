#include "platform_window.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <iostream>
#include <windowsx.h>

using kirana::utils::input::KeyAction;
using kirana::utils::input::MouseButton;
static const std::unordered_map<UINT, MouseButton> MOUSE_BUTTON_MAPPING{
    {WM_LBUTTONDOWN, MouseButton::LEFT},
    {WM_LBUTTONDBLCLK, MouseButton::LEFT},
    {WM_LBUTTONUP, MouseButton::LEFT},
    {WM_MBUTTONDOWN, MouseButton::MIDDLE},
    {WM_MBUTTONDBLCLK, MouseButton::MIDDLE},
    {WM_MBUTTONUP, MouseButton::MIDDLE},
    {WM_RBUTTONDOWN, MouseButton::RIGHT},
    {WM_RBUTTONDBLCLK, MouseButton::RIGHT},
    {WM_RBUTTONUP, MouseButton::RIGHT},
    {XBUTTON1, MouseButton::BUTTON_01},
    {XBUTTON2, MouseButton::BUTTON_02}};
static const std::unordered_map<UINT, KeyAction> KEY_ACTION_MAPPING{
    {WM_LBUTTONDOWN, KeyAction::DOWN},
    {WM_LBUTTONDBLCLK, KeyAction::DCLICK},
    {WM_LBUTTONUP, KeyAction::UP},
    {WM_MBUTTONDOWN, KeyAction::DOWN},
    {WM_MBUTTONDBLCLK, KeyAction::DCLICK},
    {WM_MBUTTONUP, KeyAction::UP},
    {WM_RBUTTONDOWN, KeyAction::DOWN},
    {WM_RBUTTONDBLCLK, KeyAction::DCLICK},
    {WM_RBUTTONUP, KeyAction::UP},
    {WM_XBUTTONDOWN, KeyAction::DOWN},
    {WM_XBUTTONDBLCLK, KeyAction::DCLICK},
    {WM_XBUTTONUP, KeyAction::UP},
    {WM_KEYDOWN, KeyAction::DOWN},
    {WM_KEYUP, KeyAction::UP}};

kirana::utils::input::ModifierKey kirana::window::PlatformWindow::
    getMouseModifierKey(WPARAM wParam)
{
    ModifierKey key = ModifierKey::NONE;
    if ((wParam & MK_SHIFT) == MK_SHIFT)
        key = key | ModifierKey::SHIFT;
    if ((wParam & MK_CONTROL) == MK_CONTROL)
        key = key | ModifierKey::CONTROL;
    // ALT is a special case. It is not stored in wParam for mouse modifier key.
    // So it needs to be checked separately.
    // Refer:
    // https://learn.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
    if (GetKeyState(VK_MENU) & 0x8000)
        key = key | ModifierKey::ALT;
    return key;
}

kirana::utils::input::MouseInput kirana::window::PlatformWindow::getMouseInput(
    UINT uMsg, WPARAM wParam)
{
    MouseInput input{};
    if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONDBLCLK ||
        uMsg == WM_XBUTTONUP)
        uMsg = GET_XBUTTON_WPARAM(wParam);
    input.button = MOUSE_BUTTON_MAPPING.at(uMsg);
    input.action = KEY_ACTION_MAPPING.at(uMsg);
    input.modifier = getMouseModifierKey(wParam);
    return input;
}

void kirana::window::PlatformWindow::handleWindowEvents(HWND hwnd, UINT uMsg,
                                                        WPARAM wParam,
                                                        LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        m_resolution[0] = static_cast<uint32_t>(width);
        m_resolution[1] = static_cast<uint32_t>(height);
        m_onWindowResize(this, m_resolution);
    }
    break;
    case WM_DESTROY: {
        m_onWindowClose(this);
        clean();
    }
    break;
    default:
        break;
    }
}

void kirana::window::PlatformWindow::handleMouseInput(HWND hwnd, UINT uMsg,
                                                      WPARAM wParam,
                                                      LPARAM lParam)
{
    if (uMsg == WM_MOUSEMOVE)
    {
        m_mousePosition[0] = GET_X_LPARAM(lParam);
        m_mousePosition[1] = GET_Y_LPARAM(lParam);
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        m_mousePosition[0] = -1;
        m_mousePosition[1] = -1;
    }
    if (uMsg == WM_MOUSEHWHEEL)
    {
        m_onScrollInput(this, 0.0,
                        static_cast<double>(GET_WHEEL_DELTA_WPARAM(wParam)));
    }

    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONUP:
        m_onMouseInput(this, getMouseInput(uMsg, wParam));
        break;
    default:
        break;
    }
}

void kirana::window::PlatformWindow::handleKeyboardInput(HWND hwnd, UINT uMsg,
                                                         WPARAM wParam,
                                                         LPARAM lParam)
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
    default:
        break;
    }
}


LRESULT CALLBACK kirana::window::PlatformWindow::WindowProc(HWND hwnd,
                                                            UINT uMsg,
                                                            WPARAM wParam,
                                                            LPARAM lParam)
{
    auto window = reinterpret_cast<kirana::window::PlatformWindow *>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
    window->handleWindowEvents(hwnd, uMsg, wParam, lParam);
    window->handleMouseInput(hwnd, uMsg, wParam, lParam);
    window->handleKeyboardInput(hwnd, uMsg, wParam, lParam);
    return CallWindowProc(window->m_prevWindowProc, hwnd, uMsg, wParam, lParam);
}

kirana::window::PlatformWindow::PlatformWindow(long windowPointer, string name,
                                               bool fullscreen, bool resizable,
                                               int width, int height)
    : Window(std::move(name), fullscreen, resizable, width, height),
      m_windowPointer{windowPointer},
      m_hwndWindowPointer{reinterpret_cast<HWND>(m_windowPointer)}
{
    // Reset window input callback to our callback function.
    m_prevWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(
        m_hwndWindowPointer, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(&PlatformWindow::WindowProc)));
    // Set the pointer of this object as user data to be used in WndProc.
    SetWindowLongPtr(m_hwndWindowPointer, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(this));

    m_resolution[0] = static_cast<uint32_t>(width);
    m_resolution[1] = static_cast<uint32_t>(height);
};

MSG msg;
void kirana::window::PlatformWindow::update() const
{
    if (GetMessage(&msg, m_hwndWindowPointer, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void kirana::window::PlatformWindow::close() const
{
    if (m_hwndWindowPointer)
        CloseWindow(m_hwndWindowPointer);
}

void kirana::window::PlatformWindow::setFocus(bool value)
{
    Window::setFocus(value);
    if (value)
        SetFocus(m_hwndWindowPointer);
}

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
