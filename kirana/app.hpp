#ifndef APP_HPP
#define APP_HPP

#include <window/window_manager.hpp>
#include <viewport/viewport.hpp>

#include <utils/logger.hpp>
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

    utils::Logger &m_logger;

    uint32_t m_windowCloseListener = UINT32_MAX;
    uint32_t m_allWindowCloseListener = UINT32_MAX;
    WindowManager m_windowManager;

    shared_ptr<Window> m_viewportWindow;
    Viewport m_viewport;

    void onWindowClosed(Window *window);

    void init();
    void update();
    void render();
    void clean();

  public:
    Application();
    ~Application();

    Application(const Application &app) = delete;
    Application &operator=(const Application &app) = delete;

    void run();
};
} // namespace kirana

#endif