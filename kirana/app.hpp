#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>
#include <viewport/viewport.hpp>

#include <utils/logger.hpp>
#include <limits>
#include <memory>

namespace kirana
{
using viewport::Viewport;
using window::Window;
using window::WindowManager;

using std::shared_ptr;

/**
     * As the name suggests, this class is the entry point of the entire
     * program. It defines the application loop in the order of initialization,
     * updating, rendering and cleaning of objects. It handles logger
     * initialization, window creation and its events, viewport creation and UI
     * related functionality.
 */
class Application
{
  private:
    bool m_isRunning = false;
    bool m_isViewportRunning = false;

    utils::Logger &m_logger;

    uint32_t m_windowCloseListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_allWindowCloseListener =
        std::numeric_limits<unsigned int>::max();
    WindowManager m_windowManager;

    shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

    /**
     * Used as a callback function for window close evevnt.
     * @param window Pointer to the window object which was closed.
     */
    void onWindowClosed(Window *window);

    /// Initializes all objects of the program.
    void init();
    /// Called every frame. Calls the update functions of viewport and windows.
    void update();
    /// Called every frame. Calls the render functions of the viewport.
    void render();
    /// Calls the cleanup functions of the objects after all windows are closed.
    void clean();

  public:
    Application();
    ~Application();

    Application(const Application &app) = delete;
    Application &operator=(const Application &app) = delete;

    /// Runs the application.
    void run();
};
} // namespace kirana

#endif