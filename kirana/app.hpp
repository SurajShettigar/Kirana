#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>
#include <viewport/viewport.hpp>

#include <memory.h>

namespace kirana
{
using viewport::Viewport;
using window::Window;
using window::WindowManager;

using std::shared_ptr;

class Application
{

  private:
    bool m_isRunning = false;
    bool m_isViewportRunning = false;

    uint32_t m_windowCloseListener;
    uint32_t m_allWindowCloseListener;
    WindowManager m_windowManager;

    shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

    void onWindowClosed(Window *window);

    Application(const Application &app) = delete;
    Application &operator=(const Application &app) = delete;

    void init();
    void update();
    void render();
    void clean();

  public:
    Application();
    ~Application();

    void run();
};
} // namespace kirana

#endif