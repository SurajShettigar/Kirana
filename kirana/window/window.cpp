#include <vulkan/vulkan.hpp>

#include "window.hpp"

void kirana::window::Window::onWindowResized(GLFWwindow *glfwWindow, int width,
                                             int height)
{
    auto *currWin = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_onWindowResize(currWin, {static_cast<uint32_t>(width),
                                            static_cast<uint32_t>(height)});
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
        glfwDestroyWindow(glfwWindow);
        currWin->m_onWindowResize.removeAllListeners();
        currWin->m_onWindowClose.removeAllListeners();
        currWin->m_onKeyboardInput.removeAllListeners();
    }
}

void kirana::window::Window::onKeyboardInput(GLFWwindow *window, int key,
                                             int scancode, int action, int mods)
{
    Window *currWin = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (currWin)
    {
        currWin->m_onKeyboardInput(
            currWin,
            input::KeyboardInput{static_cast<input::Key>(key),
                                 static_cast<input::KeyAction>(action)});
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
    glfwSetWindowSizeLimits(m_glfwWindow, m_width, m_height, mode->width,
                            mode->height);
    if (m_glfwWindow)
    {
        glfwSetWindowUserPointer(m_glfwWindow, this);
        glfwSetFramebufferSizeCallback(m_glfwWindow, Window::onWindowResized);
        glfwSetWindowCloseCallback(m_glfwWindow, Window::onWindowClosed);
        glfwSetKeyCallback(m_glfwWindow, Window::onKeyboardInput);
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

std::array<uint32_t, 2> kirana::window::Window::getWindowResolution() const
{
    int width = 0, height = 0;
    if (m_glfwWindow)
        glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    return std::array<uint32_t, 2>{static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};
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