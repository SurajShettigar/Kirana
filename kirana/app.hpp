#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>
#include <viewport/viewport.hpp>

#include <utils/logger.hpp>
#include <utils/time.hpp>
#include <limits>
#include <memory>


namespace kirana
{
namespace scene
{
class SceneManager;
}

using viewport::Viewport;
using window::Window;
using window::WindowManager;

using std::shared_ptr;

/**
 * As the m_name suggests, this class is the entry point of the entire
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
    utils::Time &m_time;

    uint32_t m_windowResizeListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_windowCloseListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_allWindowCloseListener =
        std::numeric_limits<unsigned int>::max();
    uint32_t m_keyboardInputListener = std::numeric_limits<unsigned int>::max();
    WindowManager m_windowManager;

    scene::SceneManager &m_sceneManager;

    shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

    /**
     * Used as a callback function for window resize event.
     * @param window Pointer to the window object which was resized.
     * @param resolution New resolution of the window.
     */
    void onWindowResized(Window *window, std::array<uint32_t, 2> resolution);
    /**
     * Used as a callback function for window close event.
     * @param window Pointer to the window object which was closed.
     */
    void onWindowClosed(Window *window);
    /**
     * Used as a callback function for keyboard input event.
     * @param input Struct containing key and the action performed.
     */
    void onKeyboardInput(window::input::KeyboardInput input);

    /**
     * Loads the default scene into m_currentScene object.
     * @return true if successfully loaded.
     */
    bool loadDefaultScene();

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