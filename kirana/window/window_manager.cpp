#include "window_manager.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>
using namespace std::placeholders;

void kirana::window::WindowManager::onWindowClosed(const Window &window)
{
    m_windows.erase(
        std::remove_if(m_windows.begin(), m_windows.end(),
                       [&window](Window &w) { return window == w; }),
        m_windows.end()); // Removes the window object from the list

    if (m_windows.size() == 0)
        m_onAllWindowsClosedCallback();
}

void kirana::window::WindowManager::init()
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

void kirana::window::WindowManager::update() const
{
    for (const auto &w : m_windows)
        w.update();

    glfwPollEvents();
}

void kirana::window::WindowManager::clean()
{
    glfwTerminate();
    m_isInitialized = false;
}

kirana::window::Window &kirana::window::WindowManager::createWindow(string name,
                                                                    int width,
                                                                    int height)
{
    m_windows.push_back(Window(name, width, height));
    Window &window = m_windows.at(m_windows.size()-1);
    window.setOnWindowCloseListener(
        std::bind(&WindowManager::onWindowClosed, this, _1));
    window.create();
    return window;
}

void kirana::window::WindowManager::closeWindow(const Window &window)
{
    if (m_isInitialized)
        window.close();
}

void kirana::window::WindowManager::closeAllWindows()
{
    if (m_isInitialized)
        for (const auto &w : m_windows)
            closeWindow(w);
}