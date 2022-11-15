#ifndef API_WINDOW_HPP
#define API_WINDOW_HPP

#include "window.hpp"
#include <GLFW/glfw3.h>

namespace kirana::window
{
class APIWindow : public Window
{
    friend class WindowManager;
  protected:
    GLFWwindow *m_glfwWindow = nullptr;
    /**
     * @brief Called by GLFW when a window is resized.
     * @param glfwWindow The window which got resized.
     * @param width New m_width of the window.
     * @param height New m_height of the window.
     */
    static void onWindowResized(GLFWwindow *glfwWindow, int width, int height);
    /**
     * @brief Called by GLFW when close flag of a window is set.
     *
     * @param glfwWindow The window which is being currently closed.
     */
    static void onWindowClosed(GLFWwindow *glfwWindow);
    /**
     * Called by GLFW when a keyboard button is pressed, released or held down.
     * @param window The current window on focus.
     * @param key GLFW keycode for button.
     * @param scancode Unique identifier of the button (system-specific).
     * @param action One of GLFW_RELEASE, GLFW_PRESS, GLFW_REPEAT.
     * @param mods Bit field describing which modifier keys were held down (Like
     * SHIFT, CTRL etc.).
     */
    static void onKeyboardInput(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
    /**
     * Called by GLFW when a mouse button is pressed, released or held down.
     * @param window The current window on focus.
     * @param key GLFW keycode for mouse button.
     * @param action One of GLFW_RELEASE, GLFW_PRESS, GLFW_REPEAT.
     * @param mods Bit field describing which modifier keys were held down (Like
     * SHIFT, CTRL etc.).
     */
    static void onMouseInput(GLFWwindow *window, int button, int action,
                             int mods);
    /**
     * Called by GLFW when a scrolling device is used (mouse, touchpad).
     * @param window The current window on focus.
     * @param xOffset The scroll offset along X-axis.
     * @param yOffset The scroll offset along Y-axis. Mouse scroll values will
     * be in this axis.
     */
    static void onScrollInput(GLFWwindow *window, double xOffset,
                              double yOffset);

    /// Creates the actual window from given specification.
    void create() override;
    /// Updates the window and checks for events. Called every frame.
    void update() const override;
    /// Closes the window.
    void close() const override;
    /// Cleans the window.
    void clean() const override;
  public:
    explicit APIWindow(string name = "Window", bool fullscreen = true,
                       bool resizable = false, int width = 1280,
                       int height = 720)
        : Window(std::move(name), fullscreen, resizable, width, height), m_glfwWindow{
                                                                  nullptr} {};
    ~APIWindow() override = default;
    APIWindow(const APIWindow &window) = delete;
    APIWindow &operator=(const APIWindow &window) = delete;

    inline bool operator==(const APIWindow &rhs) const
    {
        return m_glfwWindow == rhs.m_glfwWindow;
    }

    void setFocus(bool value) override;
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
                                    VkSurfaceKHR *surface) const override;

    /** Gets a vector of required extensions when creating an instance of
     * Vulkan.
     *
     * @return std::vector<const char *> A vector containing the names of the
     * required extensions.
     */
    [[nodiscard]] std::vector<const char *>
    getReqInstanceExtensionsForVulkan() const override;
};
} // namespace kirana::window

#endif