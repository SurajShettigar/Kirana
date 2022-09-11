#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>

namespace kirana
{
using window::Window;
using window::WindowManager;

class Application
{

  private:
    bool m_isRunning = false;

    WindowManager m_windowManager;

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