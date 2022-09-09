#include "window_manager.hpp"

#include <GLFW/glfw3.h>

kirana::window::WindowManager::WindowManager()
{
    if (glfwInit())
    {
        if (glfwVulkanSupported())
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            m_isInitialized = true;
        }
    }
}

kirana::window::WindowManager::~WindowManager()
{
    glfwTerminate();
    m_isInitialized = false;
}