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

using kirana::utils::input::InputManager;
using kirana::utils::input::Key;
using kirana::utils::input::KeyAction;
using kirana::utils::input::KeyboardInput;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;

/**
 * As the m_name suggests, this class is the entry point of the entire
 * program. It defines the application loop in the order of initialization,
 * updating, rendering and cleaning of objects. It handles logger
 * initialization, window creation and its events, viewport creation and UI
 * related functionality.
 */
class Application
{
  public:
    Application(const Application &app) = delete;

    inline static Application &get()
    {
        static Application instance;
        return instance;
    }

    /// Runs the application.
    void run();
    void run(long windowId, int width, int height);

    [[nodiscard]] inline std::shared_ptr<Window> getViewportWindow()
    {
        if (m_viewportWindow)
            return m_viewportWindow;
        else
            return nullptr;
    }
  private:
    bool m_isRunning = false;
    bool m_isViewportRunning = false;

    long m_windowPointer = 0;
    int m_windowWidth = 0;
    int m_windowHeight = 0;

    utils::Logger &m_logger;
    utils::Time &m_time;

    uint32_t m_windowResizeListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_windowCloseListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_allWindowCloseListener =
        std::numeric_limits<unsigned int>::max();
    uint32_t m_keyboardInputListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_mouseInputListener = std::numeric_limits<unsigned int>::max();
    uint32_t m_scrollInputListener = std::numeric_limits<unsigned int>::max();
    WindowManager m_windowManager;

    InputManager &m_inputManager;

    scene::SceneManager &m_sceneManager;

    std::shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

    Application();
    ~Application();

    /**
     * Used as a callback function for window resize event.
     * @param window Pointer to the window object which was resized.
     * @param resolution New resolution of the window.
     */
    void onWindowResized(const Window *window,
                         std::array<uint32_t, 2> resolution);
    /**
     * Used as a callback function for window close event.
     * @param window Pointer to the window object which was closed.
     */
    void onWindowClosed(const Window *window);
    /**
     * Used as a callback function for keyboard input event.
     * @param input Struct containing button, action performed and the modifier
     * button (CTRL, SHIFT,...) held.
     */
    void onKeyboardInput(KeyboardInput input);
    /**
     * Used as a callback function for mouse input event.
     * @param input Struct containing mouse button, action performed and the
     * modifier button (CTRL, SHIFT,...) held.
     */
    void onMouseInput(MouseInput input);
    /**
     * Used as a callback function for scroll input event.
     * @param xOffset offset value in X-axis.
     * @param yOffset offset value in Y-axis. (Mouse scroll).
     */
    void onScrollInput(double xOffset, double yOffset);

    /// Initializes all objects of the program.
    void init();
    /// Called every frame. Calls the update functions of viewport and windows.
    void update();
    /// Called every frame. Calls the render functions of the viewport.
    void render();
    /// Calls the cleanup functions of the objects after all windows are closed.
    void clean();
};
} // namespace kirana

#endif