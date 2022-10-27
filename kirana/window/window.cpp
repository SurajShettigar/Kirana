#include <vulkan/vulkan.hpp>

#include "window.hpp"

void kirana::window::Window::onWindowResized(GLFWwindow *glfwWindow, int width,
                                             int height)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_resolution[0] = static_cast<uint32_t>(width);
        currWin->m_resolution[1] = static_cast<uint32_t>(height);
        currWin->m_onWindowResize(currWin, currWin->m_resolution);
    }
}

void kirana::window::Window::onWindowClosed(GLFWwindow *glfwWindow)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_onWindowClose(currWin);
        glfwSetWindowUserPointer(glfwWindow, nullptr);
        glfwSetFramebufferSizeCallback(glfwWindow, nullptr);
        glfwSetWindowCloseCallback(glfwWindow, nullptr);
        glfwSetKeyCallback(glfwWindow, nullptr);
        glfwSetMouseButtonCallback(glfwWindow, nullptr);
        glfwSetScrollCallback(glfwWindow, nullptr);
        glfwDestroyWindow(glfwWindow);
        currWin->m_onWindowResize.removeAllListeners();
        currWin->m_onWindowClose.removeAllListeners();
        currWin->m_onKeyboardInput.removeAllListeners();
    }
}

void kirana::window::Window::onKeyboardInput(GLFWwindow *window, int key,
                                             int scancode, int action, int mods)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onKeyboardInput(currWin,
                                   KeyboardInput{static_cast<Key>(key),
                                                 static_cast<KeyAction>(action),
                                                 static_cast<Key>(mods)});
    }
}

void kirana::window::Window::onMouseInput(GLFWwindow *window, int button,
                                          int action, int mods)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onMouseInput(currWin,
                                MouseInput{static_cast<MouseButton>(button),
                                           static_cast<KeyAction>(action),
                                           static_cast<Key>(mods)});
    }
}

void kirana::window::Window::onScrollInput(GLFWwindow *window, double xOffset,
                   double yOffset)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onScrollInput(currWin, xOffset, yOffset);
    }
}

void kirana::window::Window::create()
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
        glfwSetFramebufferSizeCallback(m_glfwWindow, Window::onWindowResized);
        glfwSetWindowCloseCallback(m_glfwWindow, Window::onWindowClosed);
        glfwSetKeyCallback(m_glfwWindow, Window::onKeyboardInput);
        glfwSetMouseButtonCallback(m_glfwWindow, Window::onMouseInput);
        glfwSetScrollCallback(m_glfwWindow, Window::onScrollInput);

        int resX = 0, resY = 0;
        glfwGetFramebufferSize(m_glfwWindow, &resX, &resY);
        m_resolution[0] = static_cast<uint32_t>(resX);
        m_resolution[1] = static_cast<uint32_t>(resY);
    }
}

void kirana::window::Window::update() const
{
    if (!glfwWindowShouldClose(m_glfwWindow))
    {
        return;
    }
    else
        Window::onWindowClosed(m_glfwWindow);
}

void kirana::window::Window::close() const
{
    if (m_glfwWindow)
        glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
}

VkResult kirana::window::Window::getVulkanWindowSurface(
    VkInstance instance, const VkAllocationCallbacks *allocator,
    VkSurfaceKHR *surface) const
{
    return glfwCreateWindowSurface(instance, m_glfwWindow, allocator, surface);
}

std::vector<const char *> kirana::window::Window::
    getReqInstanceExtensionsForVulkan()
{
    uint32_t count = 0;
    const char **exts = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char *>(exts, exts + count);
}