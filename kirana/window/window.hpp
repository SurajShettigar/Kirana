#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <GLFW/glfw3.h>

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace kirana::window
{
using std::array;
using std::function;
using std::string;
using std::vector;

class Window
{
  private:
    GLFWwindow *m_glfwWindow;
    function<void(const Window &)> m_windowCloseCallback;

    Window(const Window &window) = delete;
    Window &operator=(const Window &window) = delete;

  public:
    string name = "Window";
    int width = 1280;
    int height = 720;

    Window(string name = "Window", int width = 1280, int height = 720);
    ~Window();

    void update() const;

    // Window listeners
    void setOnWindowCloseListener(function<void(const Window &)> callback);

    // Utility functions
    array<int, 2> getWindowResolution() const;
};
} // namespace kirana::window

#endif