#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <functional>
#include <memory.h>
#include <string>
#include <vector>

#include <event.hpp>

#include "window.hpp"

namespace kirana::window
{

using std::shared_ptr;
using std::string;
using std::vector;

using kirana::utils::Event;

class WindowManager
{
  private:
    bool m_isInitialized = false;

    vector<shared_ptr<Window>> m_windows;

    Event<Window *> m_onWindowCloseEvent;
    Event<> m_onAllWindowsClosedEvent;

    WindowManager(const WindowManager &window) = delete;
    WindowManager &operator=(const WindowManager &window) = delete;

    void onWindowClosed(Window *window);

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
     * @brief Adds the callback function which is called when a window is
     * closed.
     *
     * @param callback The function which will be called.
     * @return uint32_t
     */
    inline uint32_t addOnWindowCloseListener(
        const std::function<void(Window *)> &callback)
    {
        return m_onWindowCloseEvent.addListener(callback);
    }
    /**
     * @brief Removes the callback function with given identifier from being
     * called after the event.
     * 
     * @param callbackID 
     */
    inline void removeOnWindowCloseListener(uint32_t callbackID)
    {
        m_onWindowCloseEvent.removeListener(callbackID);
    }

    /**
     * @brief Adds the callback function which is called when all windows are
     * closed. Useful to terminate the app.
     *
     * @param callback The function which will be called.
     * @return uint32_t Unique identifier for the callback. Can be used later as
     * an argument to remove the listener.
     */
    inline uint32_t addOnAllWindowsClosedListener(
        const std::function<void(void)> &callback)
    {
        return m_onAllWindowsClosedEvent.addListener(callback);
    }
    /**
     * @brief Removes the callback function with given identifier from being
     * called after the event.
     *
     * @param callbackID Unique identifier of th callback function.
     */
    inline void removeOnAllWindowsClosedListener(uint32_t callbackID)
    {
        m_onAllWindowsClosedEvent.removeListener(callbackID);
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
};
} // namespace kirana::window

#endif