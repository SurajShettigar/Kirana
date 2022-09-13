#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>
#include <viewport/viewport.hpp>

#include <memory.h>

namespace kirana
{
using window::Window;
using window::WindowManager;
using viewport::Viewport;

using std::shared_ptr;

class Application
{

  private:
    bool m_isRunning = false;

    WindowManager m_windowManager;
    shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

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