#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <vector>

#include "window.hpp"

namespace kirana::window
{

using std::vector;

class WindowManager
{
  private:
    bool m_isInitialized = false;

    vector<Window> m_windows;

    WindowManager(const WindowManager &window) = delete;
    WindowManager &operator=(const WindowManager &window) = delete;

    void onWindowClosed(const Window& window);
  public:
    WindowManager();
    ~WindowManager();

    bool createWindow(Window& window);
    bool destroyWindow(const Window& window);
};
} // namespace kirana::window

#endif