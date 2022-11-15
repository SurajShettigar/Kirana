#include <vulkan/vulkan.h>
#include "api_window.hpp"

void kirana::window::APIWindow::onWindowResized(GLFWwindow *glfwWindow,
                                                int width, int height)
{
    auto *currWin =
        static_cast<APIWindow *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_resolution[0] = static_cast<uint32_t>(width);
        currWin->m_resolution[1] = static_cast<uint32_t>(height);
        currWin->m_onWindowResize(currWin, currWin->m_resolution);
    }
}

void kirana::window::APIWindow::onWindowClosed(GLFWwindow *glfwWindow)
{
    auto *currWin =
        static_cast<APIWindow *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_onWindowClose(currWin);
        currWin->clean();
    }
}

void kirana::window::APIWindow::onKeyboardInput(GLFWwindow *window, int key,
                                                int scancode, int action,
                                                int mods)
{
    auto *currWin = static_cast<APIWindow *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onKeyboardInput(
            currWin,
            KeyboardInput{static_cast<Key>(key), static_cast<KeyAction>(action),
                          static_cast<ModifierKey>(mods)});
    }
}

void kirana::window::APIWindow::onMouseInput(GLFWwindow *window, int button,
                                             int action, int mods)
{
    auto *currWin = static_cast<APIWindow *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onMouseInput(currWin,
                                MouseInput{static_cast<MouseButton>(button),
                                           static_cast<KeyAction>(action),
                                           static_cast<ModifierKey>(mods)});
    }
}

void kirana::window::APIWindow::onScrollInput(GLFWwindow *window,
                                              double xOffset, double yOffset)
{
    auto *currWin = static_cast<APIWindow *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onScrollInput(currWin, xOffset, yOffset);
    }
}

void kirana::window::APIWindow::create()
{
    glfwWindowHint(GLFW_RESIZABLE, m_resizable);
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (m_fullscreen)
    {
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        m_glfwWindow = glfwCreateWindow(mode->width, mode->height,
                                        m_name.c_str(), nullptr, nullptr);
    }
    else
        m_glfwWindow = glfwCreateWindow(m_width, m_height, m_name.c_str(),
                                        nullptr, nullptr);
    if (m_glfwWindow)
    {
        glfwSetWindowSizeLimits(m_glfwWindow, m_width, m_height, mode->width,
                                mode->height);
        glfwSetWindowUserPointer(m_glfwWindow, this);
        glfwSetFramebufferSizeCallback(m_glfwWindow,
                                       APIWindow::onWindowResized);
        glfwSetWindowCloseCallback(m_glfwWindow, APIWindow::onWindowClosed);
        glfwSetKeyCallback(m_glfwWindow, APIWindow::onKeyboardInput);
        glfwSetMouseButtonCallback(m_glfwWindow, APIWindow::onMouseInput);
        glfwSetScrollCallback(m_glfwWindow, APIWindow::onScrollInput);

        int resX = 0, resY = 0;
        glfwGetFramebufferSize(m_glfwWindow, &resX, &resY);
        m_resolution[0] = static_cast<uint32_t>(resX);
        m_resolution[1] = static_cast<uint32_t>(resY);
    }
}

void kirana::window::APIWindow::update() const
{
    if (!glfwWindowShouldClose(m_glfwWindow))
    {
        return;
    }
    else if (m_glfwWindow != nullptr)
        APIWindow::onWindowClosed(m_glfwWindow);
}

void kirana::window::APIWindow::close() const
{
    if (m_glfwWindow)
        glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
}

void kirana::window::APIWindow::clean() const
{
    glfwSetWindowUserPointer(m_glfwWindow, nullptr);
    glfwSetFramebufferSizeCallback(m_glfwWindow, nullptr);
    glfwSetWindowCloseCallback(m_glfwWindow, nullptr);
    glfwSetKeyCallback(m_glfwWindow, nullptr);
    glfwSetMouseButtonCallback(m_glfwWindow, nullptr);
    glfwSetScrollCallback(m_glfwWindow, nullptr);
    glfwDestroyWindow(m_glfwWindow);
    Window::clean();
}


void kirana::window::APIWindow::setFocus(bool value)
{
    Window::setFocus(value);
    if (value)
        glfwFocusWindow(m_glfwWindow);
}

VkResult kirana::window::APIWindow::getVulkanWindowSurface(
    VkInstance instance, const VkAllocationCallbacks *allocator,
    VkSurfaceKHR *surface) const
{
    return glfwCreateWindowSurface(instance, m_glfwWindow, allocator, surface);
}

std::vector<const char *> kirana::window::APIWindow::
    getReqInstanceExtensionsForVulkan() const
{
    uint32_t count = 0;
    const char **exts = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char *>(exts, exts + count);
}