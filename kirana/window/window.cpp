#include <vulkan/vulkan.hpp>

#include "window.hpp"

void kirana::window::Window::onWindowClosed(GLFWwindow *glfwWindow)
{
    Window *currWin =
        static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_onWindowCloseEvent(currWin);
        glfwSetWindowUserPointer(glfwWindow, nullptr);
        glfwSetWindowCloseCallback(glfwWindow, nullptr);
        glfwSetKeyCallback(glfwWindow, nullptr);
        glfwDestroyWindow(glfwWindow);
        currWin->removeAllOnWindowCloseListener();
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
    m_glfwWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (m_glfwWindow)
    {
        glfwSetWindowUserPointer(m_glfwWindow, this);
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

std::array<int, 2> kirana::window::Window::getWindowResolution() const
{
    int width = 0, height = 0;
    if (m_glfwWindow)
        glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    return std::array<int, 2>{width, height};
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