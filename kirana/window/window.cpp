#include <vulkan/vulkan.hpp>

#include "window.hpp"

void kirana::window::Window::onWindowClosed(GLFWwindow *glfwWindow)
{
    Window *currWin =
        static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (currWin)
    {
        currWin->m_windowCloseCallback(currWin);
        glfwSetWindowUserPointer(glfwWindow, nullptr);
        glfwSetWindowCloseCallback(glfwWindow, nullptr);
        glfwDestroyWindow(glfwWindow);
    }
}

void kirana::window::Window::create()
{
    m_glfwWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (m_glfwWindow)
    {
        glfwSetWindowUserPointer(m_glfwWindow, this);
        glfwSetWindowCloseCallback(m_glfwWindow, Window::onWindowClosed);
    }
}

void kirana::window::Window::update() const
{
    if (glfwWindowShouldClose(m_glfwWindow))
        return;
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