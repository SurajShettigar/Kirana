#include "platform_window.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <iostream>
#include <windowsx.h>
#include <hidusage.h>

using kirana::utils::input::Key;
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
static const std::unordered_map<WPARAM, Key> KEY_MAPPING{
    {0x07, Key::UNKNOWN},
    {VK_SPACE, Key::SPACE},
    {VK_OEM_7, Key::APOSTROPHE},
    {VK_OEM_COMMA, Key::COMMA},
    {VK_OEM_MINUS, Key::MINUS},
    {VK_OEM_PERIOD, Key::PERIOD},
    {VK_OEM_2, Key::SLASH},
    {0x30, Key::NUM_0},
    {0x31, Key::NUM_1},
    {0x32, Key::NUM_2},
    {0x33, Key::NUM_3},
    {0x34, Key::NUM_4},
    {0x35, Key::NUM_5},
    {0x36, Key::NUM_6},
    {0x37, Key::NUM_7},
    {0x38, Key::NUM_8},
    {0x39, Key::NUM_9},
    {VK_OEM_1, Key::SEMICOLON},
    {VK_OEM_PLUS, Key::EQUAL},
    {0x41, Key::A},
    {0x42, Key::B},
    {0x43, Key::C},
    {0x44, Key::D},
    {0x45, Key::E},
    {0x46, Key::F},
    {0x47, Key::G},
    {0x48, Key::H},
    {0x49, Key::I},
    {0x4A, Key::J},
    {0x4B, Key::K},
    {0x4C, Key::L},
    {0x4D, Key::M},
    {0x4E, Key::N},
    {0x4F, Key::O},
    {0x50, Key::P},
    {0x51, Key::Q},
    {0x52, Key::R},
    {0x53, Key::S},
    {0x54, Key::T},
    {0x55, Key::U},
    {0x56, Key::V},
    {0x57, Key::W},
    {0x58, Key::X},
    {0x59, Key::Y},
    {0x5A, Key::Z},
    {VK_OEM_4, Key::LEFT_BRACKET},
    {VK_OEM_5, Key::BACKSLASH},
    {VK_OEM_6, Key::RIGHT_BRACKET},
    {VK_OEM_3, Key::GRAVE_ACCENT},
    {VK_ESCAPE, Key::ESCAPE},
    {VK_RETURN, Key::ENTER},
    {VK_TAB, Key::TAB},
    {VK_BACK, Key::BACKSPACE},
    {VK_INSERT, Key::INSERT},
// DELETE is defined in windows.h. We un-define it so that it does not clash
// with our Key::DELETE enum value.
#ifdef DELETE
#undef DELETE
    {VK_DELETE, Key::DELETE},
#define DELETE (0x00010000L)
#endif
    {VK_RIGHT, Key::RIGHT},
    {VK_LEFT, Key::LEFT},
    {VK_DOWN, Key::DOWN},
    {VK_UP, Key::UP},
    {VK_PRIOR, Key::PAGE_UP},
    {VK_NEXT, Key::PAGE_DOWN},
    {VK_HOME, Key::HOME},
    {VK_END, Key::END},
    {VK_CAPITAL, Key::CAPS_LOCK},
    {VK_SCROLL, Key::SCROLL_LOCK},
    {VK_NUMLOCK, Key::NUM_LOCK},
    {VK_SNAPSHOT, Key::PRINT_SCREEN},
    {VK_PAUSE, Key::PAUSE},
    {VK_F1, Key::F1},
    {VK_F2, Key::F2},
    {VK_F3, Key::F3},
    {VK_F4, Key::F4},
    {VK_F5, Key::F5},
    {VK_F6, Key::F6},
    {VK_F7, Key::F7},
    {VK_F8, Key::F8},
    {VK_F9, Key::F9},
    {VK_F10, Key::F10},
    {VK_F11, Key::F11},
    {VK_F12, Key::F12},
    {VK_F13, Key::F13},
    {VK_F14, Key::F14},
    {VK_F15, Key::F15},
    {VK_F16, Key::F16},
    {VK_F17, Key::F17},
    {VK_F18, Key::F18},
    {VK_F19, Key::F19},
    {VK_F20, Key::F20},
    {VK_F21, Key::F21},
    {VK_F22, Key::F22},
    {VK_F23, Key::F23},
    {VK_F24, Key::F24},
    {VK_NUMPAD0, Key::NUMPAD_0},
    {VK_NUMPAD1, Key::NUMPAD_1},
    {VK_NUMPAD2, Key::NUMPAD_2},
    {VK_NUMPAD3, Key::NUMPAD_3},
    {VK_NUMPAD4, Key::NUMPAD_4},
    {VK_NUMPAD5, Key::NUMPAD_5},
    {VK_NUMPAD6, Key::NUMPAD_6},
    {VK_NUMPAD7, Key::NUMPAD_7},
    {VK_NUMPAD8, Key::NUMPAD_8},
    {VK_NUMPAD9, Key::NUMPAD_9},
    {VK_DECIMAL, Key::NUMPAD_DECIMAL},
    {VK_DIVIDE, Key::NUMPAD_DIVIDE},
    {VK_MULTIPLY, Key::NUMPAD_MULTIPLY},
    {VK_SUBTRACT, Key::NUMPAD_SUBTRACT},
    {VK_ADD, Key::NUMPAD_ADD},
    {VK_SEPARATOR, Key::NUMPAD_EQUAL},
    {VK_LSHIFT, Key::LEFT_SHIFT},
    {VK_LCONTROL, Key::LEFT_CONTROL},
    {VK_LMENU, Key::LEFT_ALT},
    {VK_LWIN, Key::LEFT_SUPER},
    {VK_RSHIFT, Key::RIGHT_SHIFT},
    {VK_RCONTROL, Key::RIGHT_CONTROL},
    {VK_RMENU, Key::RIGHT_ALT},
    {VK_RWIN, Key::RIGHT_SUPER},
    {VK_APPS, Key::MENU}};

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

void kirana::window::PlatformWindow::handleWindowEvents(UINT uMsg,
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

void kirana::window::PlatformWindow::handleMouseInput(UINT uMsg, WPARAM wParam,
                                                      LPARAM lParam)
{
    if (uMsg == WM_MOUSEMOVE)
    {
        m_isCursorInside = true;
        if (!m_isFocused)
            setFocus(true);
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        m_isCursorInside = false;
        if (m_isFocused)
            setFocus(false);
    }
//    if (uMsg == WM_MOUSEMOVE || uMsg == WM_MOUSELEAVE || uMsg)
//    {
//        m_cursorPosition[0] = GET_X_LPARAM(lParam);
//        m_cursorPosition[1] = GET_Y_LPARAM(lParam);
//    }
    // Raw mouse input.
    /*if (uMsg == WM_INPUT)
    {
        UINT dwSize = sizeof(RAWINPUT);
        static BYTE lpb[sizeof(RAWINPUT)];

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
                    sizeof(RAWINPUTHEADER));

    auto raw = (RAWINPUT *)lpb;
    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        m_cursorPosition[0] = raw->data.mouse.lLastX;
        m_cursorPosition[1] = raw->data.mouse.lLastY;
    }
}*/
    if (uMsg == WM_MOUSEHWHEEL || uMsg == WM_MOUSEWHEEL)
    {
        double x = 0.0, y = 0.0;
        // The values are normalized to [-1.0, 1.0]. Win32 API provides values
        // with max range of 120 for high-resolution scroll-wheels.
        if (uMsg == WM_MOUSEHWHEEL)
            x = static_cast<double>(GET_WHEEL_DELTA_WPARAM(wParam)) / 120.0;
        if (uMsg == WM_MOUSEWHEEL)
            y = static_cast<double>(GET_WHEEL_DELTA_WPARAM(wParam)) / 120.0;
        m_onScrollInput(this, x, y);
    }

    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ||
        uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONDOWN ||
        uMsg == WM_MBUTTONDBLCLK || uMsg == WM_MBUTTONUP ||
        uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK ||
        uMsg == WM_RBUTTONUP || uMsg == WM_XBUTTONDOWN ||
        uMsg == WM_XBUTTONDBLCLK || uMsg == WM_XBUTTONUP)
    {
        kirana::utils::input::MouseInput input{};

        if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONDBLCLK ||
            uMsg == WM_XBUTTONUP)
        {
            input.button = MOUSE_BUTTON_MAPPING.at(GET_XBUTTON_WPARAM(wParam));
        }
        else if (MOUSE_BUTTON_MAPPING.find(uMsg) != MOUSE_BUTTON_MAPPING.end())
        {
            input.button = MOUSE_BUTTON_MAPPING.at(uMsg);
        }
        input.action = KEY_ACTION_MAPPING.at(uMsg);
        input.modifier = getMouseModifierKey(wParam);
        m_onMouseInput(this, input);
    }
}

void kirana::window::PlatformWindow::handleKeyboardInput(UINT uMsg,
                                                         WPARAM wParam,
                                                         LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
    {
        kirana::utils::input::KeyboardInput input{};
        if (KEY_MAPPING.find(wParam) != KEY_MAPPING.end())
            input.key = KEY_MAPPING.at(wParam);

        if ((HIWORD(lParam) & KF_UP) == KF_UP)
            input.action = KeyAction::UP;
        else if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
            input.action = KeyAction::HELD;
        else
            input.action = KeyAction::DOWN;

        input.modifier = ModifierKey::NONE;
        m_onKeyboardInput(this, input);
    }
}


LRESULT CALLBACK kirana::window::PlatformWindow::WindowProc(HWND hwnd,
                                                            UINT uMsg,
                                                            WPARAM wParam,
                                                            LPARAM lParam)
{
    auto window = reinterpret_cast<kirana::window::PlatformWindow *>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
    window->handleWindowEvents(uMsg, wParam, lParam);
    window->handleMouseInput(uMsg, wParam, lParam);
    window->handleKeyboardInput(uMsg, wParam, lParam);
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

    // Add hover and leave events for mouse.
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hwndWindowPointer;
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);

    // Register mouse to get high-precision mouse positions.
    /*    RAWINPUTDEVICE mouseInputDevice[1];
        mouseInputDevice[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        mouseInputDevice[0].usUsage = HID_USAGE_GENERIC_MOUSE;
        mouseInputDevice[0].dwFlags = RIDEV_NOLEGACY;
        mouseInputDevice[0].hwndTarget = m_hwndWindowPointer;
        RegisterRawInputDevices(mouseInputDevice, 1,
        sizeof(mouseInputDevice[0]));*/

    m_resolution[0] = static_cast<uint32_t>(width);
    m_resolution[1] = static_cast<uint32_t>(height);
}

POINT  point;
RECT rect;
void kirana::window::PlatformWindow::update()
{
    if(GetCursorPos(&point))
    {
        if(GetWindowRect(m_hwndWindowPointer, &rect))
        {
            m_cursorPosition[0] = static_cast<int>(point.x - rect.left);
            m_cursorPosition[1] = static_cast<int>(point.y - rect.top);
        }
    }
}

void kirana::window::PlatformWindow::close()
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
