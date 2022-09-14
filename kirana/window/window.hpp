#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <GLFW/glfw3.h>

#include <array>
#include <functional>
#include <iostream>
#include <memory.h>
#include <string>
#include <vector>

// Forward declaration of Vulkan API
enum VkResult;
struct VkInstance_T;
typedef VkInstance_T *VkInstance;
struct VkAllocationCallbacks;
struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T *VkSurfaceKHR;

namespace kirana::window
{
using std::array;
using std::function;
using std::shared_ptr;
using std::string;
using std::vector;

class Window
{
  friend class WindowManager;
  private:
    GLFWwindow *m_glfwWindow = nullptr;
    function<void(Window *)> m_windowCloseCallback = nullptr;
    /**
     * @brief Called by GLFW when close flag of a window is set
     *
     * @param glfwWindow The window which is being currently closed
     */
    static void onWindowClosed(GLFWwindow *glfwWindow);

    Window(const Window &window) = delete;
    Window &operator=(const Window &window) = delete;

    /**
     * @brief Sets the callback function which is called when the window is
     * closed.
     *
     * @param callback The function which will to be called.
     */
    inline void setOnWindowCloseListener(function<void(Window *)> callback)
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

  public:
    string name = "Window";
    int width = 1280;
    int height = 720;

    Window(string name = "Window", int width = 1280, int height = 720)
        : m_glfwWindow{nullptr}, name{name}, width{width}, height{height} {};
    ~Window() = default;

    inline bool operator==(const Window &rhs) const
    {
        return m_glfwWindow == rhs.m_glfwWindow;
    }

    /**
     * @brief Get the pixel resolution of the framebuffer of the
     * window.
     *
     * @return array<int, 2> {width, height}
     */
    array<int, 2> getWindowResolution() const;

    /**
     * @brief Get the Vulkan Window Surface object for the current window.
     *
     * @param instance Vulkan instance.
     * @param allocator Vulkan allocator object.
     * @param surface Vulkan surface object to be initialized.
     * @return VkResult Result of the operation.
     */
    VkResult getVulkanWindowSurface(VkInstance instance,
                                    const VkAllocationCallbacks *allocator,
                                    VkSurfaceKHR *surface) const;
};
} // namespace kirana::window

#endif