#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>
#include <input_manager.hpp>

#include "api_window.hpp"
#include "platform_window.hpp"

namespace kirana::window
{

using std::string;
using std::vector;

using kirana::utils::Event;
using kirana::utils::input::KeyboardInput;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;

class WindowManager
{
  private:
    bool m_isInitialized = false;
    std::array<int, 2> m_mousePosition;

    vector<std::shared_ptr<Window>> m_windows;
    const Window *m_currentWindow;

    Event<const Window *, std::array<uint32_t, 2>> m_onWindowResize;
    Event<const Window *> m_onWindowClose;
    Event<> m_onAllWindowsClose;
    Event<KeyboardInput> m_onKeyboardInput;
    Event<MouseInput> m_onMouseInput;
    Event<double, double> m_onScrollInput;

    void onWindowClosed(const Window *window);

  public:
    WindowManager() = default;
    ~WindowManager() = default;
    WindowManager(const WindowManager &window) = delete;
    WindowManager &operator=(const WindowManager &window) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::array<int, 2> &mousePosition = m_mousePosition;

    inline bool isAnyWindowOpen()
    {
        return !m_windows.empty();
    }

    /// Initializes the manager and GLFW API
    void init();
    /// Updates the windows and checks for input events. Called every frame.
    void update();
    /// Cleans up the manager and terminates GLFW.
    void clean();

    /**
     *@brief Adds the callback function which is called when a window is
     * resized.
     * @param callback The function which will be called.
     * @return uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnWindowResizeListener(
        const std::function<void(const Window *, std::array<uint32_t, 2>)>
            &callback)
    {
        return m_onWindowResize.addListener(callback);
    }
    /**
     * @brief Removes the callback function fow window close with given
     * identifier from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnWindowResizeListener(uint32_t callbackID)
    {
        m_onWindowResize.removeListener(callbackID);
    }

    /**
     * @brief Adds the callback function which is called when a window is
     * closed.
     *
     * @param callback The function which will be called.
     * @return uint32_t
     */
    inline uint32_t addOnWindowCloseListener(
        const std::function<void(const Window *)> &callback)
    {
        return m_onWindowClose.addListener(callback);
    }
    /**
     * @brief Removes the callback function with given identifier from being
     * called after the event.
     *
     * @param callbackID
     */
    inline void removeOnWindowCloseListener(uint32_t callbackID)
    {
        m_onWindowClose.removeListener(callbackID);
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
        return m_onAllWindowsClose.addListener(callback);
    }
    /**
     * @brief Removes the callback function with given identifier from being
     * called after the event.
     *
     * @param callbackID Unique identifier of th callback function.
     */
    inline void removeOnAllWindowsClosedListener(uint32_t callbackID)
    {
        m_onAllWindowsClose.removeListener(callbackID);
    }

    /** Adds a callback function for keyboard input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnKeyboardInputEventListener(
        const std::function<void(KeyboardInput)> &callback)
    {
        return m_onKeyboardInput.addListener(callback);
    }
    /** Removes the callback function for keyboard input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnKeyboardInputEventListener(uint32_t callbackID)
    {
        m_onKeyboardInput.removeListener(callbackID);
    }
    /** Adds a callback function for mouse input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnMouseInputEventListener(
        const std::function<void(MouseInput)> &callback)
    {
        return m_onMouseInput.addListener(callback);
    }
    /** Removes the callback function for mouse input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnMouseInputEventListener(uint32_t callbackID)
    {
        m_onMouseInput.removeListener(callbackID);
    }
    /** Adds a callback function for scroll input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnScrollInputEventListener(
        const std::function<void(double, double)> &callback)
    {
        return m_onScrollInput.addListener(callback);
    }
    /** Removes the callback function for scroll input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnScrollInputEventListener(uint32_t callbackID)
    {
        m_onScrollInput.removeListener(callbackID);
    }

    /**
     * Sets the focus to the given window. The window will then receive all the
     * input events.
     * @param window The pointer to the window.
     */
    void setFocus(const Window *window);

    /**
     * @brief Creates a window with given specifications.
     *
     * @param name Name of the window.
     * @param fullscreen If the window should open in fullscreen mode.
     * @param resizable If the window should be resizable.
     * @param width Width of the window.
     * @param height Height of the window.
     * @return Window* Pointer to the window object.
     */
    [[nodiscard]] std::shared_ptr<Window> createWindow(
        const string &name = "Window", bool fullscreen = true,
        bool resizable = false, int width = 1280, int height = 720);

    /** @brief Creates a Window object from the given platform specific window
     * pointer.
     *
     * @param windowPointer The pointer to the platform specific window.
     * @param name Name of the window.
     * @param fullscreen If the window is in fullscreen mode.
     * @param resizable If the window is resizable.
     * @param width Width of the window.
     * @param height Height of the window.
     * @return Window*  Shared pointer to the window object.
     */
    [[nodiscard]] std::shared_ptr<Window> createWindow(
        long windowPointer, const string &name = "Window",
        bool fullscreen = true, bool resizable = false, int width = 1280,
        int height = 720);
    /**
     * @brief Close the given window.
     *
     * @param window Window to be closed.
     */
    void closeWindow(const Window *window) const;
    /**
     * @brief Closes all the windows.
     *
     */
    void closeAllWindows();
};
} // namespace kirana::window

#endif