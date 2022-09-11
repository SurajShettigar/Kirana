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
    GLFWwindow *m_glfwWindow = nullptr;
    function<void(const Window &)> m_windowCloseCallback = nullptr;
    /**
     * @brief Called by GLFW when close flag of a window is set
     *
     * @param glfwWindow The window which is being currently closed
     */
    static void onWindowClosed(GLFWwindow *glfwWindow);

  public:
    string name = "Window";
    int width = 1280;
    int height = 720;
    Window(string name = "Window", int width = 1280, int height = 720)
        : m_glfwWindow{nullptr}, m_windowCloseCallback{nullptr}, name{name},
          width{width}, height{height} {};
    ~Window() = default;

    Window(const Window &window)
    {
        m_glfwWindow = window.m_glfwWindow;
        if (m_glfwWindow)
            glfwSetWindowUserPointer(
                m_glfwWindow,
                this); // Transfer the Window pointer to this Window object
        m_windowCloseCallback = window.m_windowCloseCallback;
        name = window.name;
        width = window.width;
        height = window.height;
    }
    Window &operator=(const Window &window)
    {
        m_glfwWindow = window.m_glfwWindow;
        if (m_glfwWindow)
            glfwSetWindowUserPointer(
                m_glfwWindow,
                this); // Transfer the Window pointer to this Window object
        m_windowCloseCallback = window.m_windowCloseCallback;
        name = window.name;
        width = window.width;
        height = window.height;

        return *this;
    }

    inline bool operator==(const Window &rhs) const
    {
        return m_glfwWindow == rhs.m_glfwWindow;
    }

    /**
     * @brief Sets the callback function which is called when the window is
     * closed.
     *
     * @param callback The function which will to be called.
     */
    inline void setOnWindowCloseListener(
        function<void(const Window &)> callback)
    {
        m_windowCloseCallback = callback;
    }


    /**
     * @brief Creates the actual window from given specification
     *
     */
    void create();
    /**
     * @brief Updates the window and checks for events. Called every frame.
     *
     */
    void update() const;
    /**
     * @brief Closes the window.
     *
     */
    void close() const;

    /**
     * @brief Get the pixel resolution of the framebuffer of the
     * window.
     *
     * @return array<int, 2> {width, height}
     */
    array<int, 2> getWindowResolution() const;
};
} // namespace kirana::window

#endif