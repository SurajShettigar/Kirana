#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "window_manager.hpp"

using namespace std::placeholders;

void kirana::window::WindowManager::onWindowClosed(Window *window)
{
    m_onWindowCloseEvent(window);

    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                   [&window](shared_ptr<Window> w) {
                                       return *window == *w.get();
                                   }),
                    m_windows.end()); // Removes the window object from the list

    if (m_windows.size() == 0)
        m_onAllWindowsClosedEvent();
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
        w->update();

    glfwPollEvents();
}

void kirana::window::WindowManager::clean()
{
    glfwTerminate();
    m_isInitialized = false;
}

std::shared_ptr<kirana::window::Window> kirana::window::WindowManager::
    createWindow(string name, int width, int height)
{
    shared_ptr<Window> window = std::make_shared<Window>(name, width, height);
    window.get()->setOnWindowCloseListener(
        std::bind(&WindowManager::onWindowClosed, this, _1));
    window->create();
    m_windows.emplace_back(window);
    return window;
}

void kirana::window::WindowManager::closeWindow(shared_ptr<Window> window)
{
    if (m_isInitialized)
        window->close();
}

void kirana::window::WindowManager::closeAllWindows()
{
    if (m_isInitialized)
        for (const auto &w : m_windows)
            closeWindow(w);
}

std::vector<const char *> kirana::window::WindowManager::
    getReqInstanceExtensionsForVulkan() const
{
    uint32_t count = 0;
    const char **exts = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char *>(exts, exts + count);
}