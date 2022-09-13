#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <functional>
#include <memory.h>
#include <optional>
#include <string>
#include <vector>

#include "window.hpp"

namespace kirana::window
{

using std::shared_ptr;
using std::string;
using std::vector;

class WindowManager
{
  private:
    bool m_isInitialized = false;

    vector<shared_ptr<Window>> m_windows;

    std::function<void(void)> m_onAllWindowsClosedCallback = nullptr;

    WindowManager(const WindowManager &window) = delete;
    WindowManager &operator=(const WindowManager &window) = delete;

    void onWindowClosed(Window* window);

  public:
    WindowManager() = default;
    ~WindowManager()
    {
        clean();
    }

    /**
     * @brief Initializes the manager and GLFW API
     *
     */
    void init();
    /**
     * @brief Updates the windows and checks for input events. Called every
     * frame.
     *
     */
    void update() const;
    /**
     * @brief Cleans up the manager and terminates GLFW
     *
     */
    void clean();

    /**
     * @brief Sets the callback function which is called when all windows are
     * closed. Useful to terminate the app.
     *
     * @param callback The function which will be called.
     */
    inline void setOnAllWindowsClosedListener(
        std::function<void(void)> callback)
    {
        m_onAllWindowsClosedCallback = callback;
    }

    /**
     * @brief Creates a window with given specifications.
     *
     * @param name Name of the window.
     * @param width Width of the window.
     * @param height Height of the window.
     * @return std::shared_ptr<kirana::window::Window>  Shared pointer to the
     * window object.
     */
    std::shared_ptr<kirana::window::Window> createWindow(string name = "Window",
                                                         int width = 1280,
                                                         int height = 720);
    /**
     * @brief Close the given window.
     *
     * @param window Window to be closed.
     */
    void closeWindow(shared_ptr<Window> window);
    /**
     * @brief Closes all the windows.
     *
     */
    void closeAllWindows();

    std::vector<const char *> getReqInstanceExtensionsForVulkan() const;
};
} // namespace kirana::window

#endif