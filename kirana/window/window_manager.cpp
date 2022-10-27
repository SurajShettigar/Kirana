#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "window_manager.hpp"

using namespace std::placeholders;

void kirana::window::WindowManager::onWindowClosed(Window *window)
{
    m_onWindowClose(window);
    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                   [&window](const shared_ptr<Window> &w) {
                                       return *window == *w;
                                   }),
                    m_windows.end()); // Removes the window object from the list

    if (m_windows.empty())
        m_onAllWindowsClose();
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

void kirana::window::WindowManager::update()
{
    if (!m_windows.empty())
        glfwPollEvents();
    for (const auto &w : m_windows)
        w->update();
}

void kirana::window::WindowManager::clean()
{
    glfwTerminate();
    m_isInitialized = false;
}

std::shared_ptr<kirana::window::Window> kirana::window::WindowManager::
    createWindow(const string &name, bool fullscreen, bool resizable, int width,
                 int height)
{
    shared_ptr<Window> window =
        std::make_shared<Window>(name, fullscreen, resizable, width, height);

    window->addOnWindowResizeListener(
        [&](Window *window, std::array<uint32_t, 2> resolution) {
            this->m_onWindowResize(window, resolution);
        });

    window->addOnWindowCloseListener(
        std::bind(&WindowManager::onWindowClosed, this, _1));

    window->addOnKeyboardInputEventListener(
        [&](Window *window, KeyboardInput input) {
            this->m_onKeyboardInput(input);
        });
    window->addOnMouseInputEventListener(
        [&](Window *window, MouseInput input) { this->m_onMouseInput(input); });
    window->addOnScrollInputEventListener(
        [&](Window *window, double xOffset, double yOffset) {
            this->m_onScrollInput(xOffset, yOffset);
        });

    window->create();
    m_windows.emplace_back(window);
    return window;
}

void kirana::window::WindowManager::closeWindow(
    const shared_ptr<Window> &window) const
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