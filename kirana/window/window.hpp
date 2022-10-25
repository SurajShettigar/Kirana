#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <GLFW/glfw3.h>

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <event.hpp>
#include "input_constants.hpp"

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
    utils::Event<Window *, std::array<uint32_t, 2>> m_onWindowResize;
    utils::Event<Window *> m_onWindowClose;
    utils::Event<Window *, input::KeyboardInput> m_onKeyboardInput;

    string m_name = "Window";
    bool m_fullscreen = true;
    bool m_resizable = false;
    int m_width = 1280;
    int m_height = 720;
    std::array<uint32_t, 2> m_resolution{static_cast<uint32_t>(m_width),
                                         static_cast<uint32_t>(m_height)};
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
     * Called by GLFW when a keyboard key is pressed, released or held down.
     * @param window The current window on focus.
     * @param key GLFW keycode for key.
     * @param scancode Unique identifier of the key (system-specific).
     * @param action One of GLFW_RELEASE, GLFW_PRESS, GLFW_REPEAT.
     * @param mods Bit field describing which modifier keys were held down (Like
     * SHIFT, CTRL etc.).
     */
    static void onKeyboardInput(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
    /**
     *@brief Adds the callback function which is called when a window is
     * resized.
     * @param callback The function which will be called.
     * @return uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnWindowResizeListener(
        const std::function<void(Window *, std::array<uint32_t, 2>)> &callback)
    {
        return m_onWindowResize.addListener(callback);
    }
    /**
     * @brief Removes the callback function fow window close with given
     * identifier from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnWindowResizeListener(uint32_t callbackID)
    {
        m_onWindowResize.removeListener(callbackID);
    }

    /**
     * @brief Adds the callback function which is called when a window is
     * closed.
     *
     * @param callback The function which will be called.
     * @return uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnWindowCloseListener(
        const std::function<void(Window *)> &callback)
    {
        return m_onWindowClose.addListener(callback);
    }
    /**
     * @brief Removes the callback function fow window close with given
     * identifier from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnWindowCloseListener(uint32_t callbackID)
    {
        m_onWindowClose.removeListener(callbackID);
    }

    /** Adds a callback function for keyboard input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnKeyboardInputEventListener(
        const std::function<void(Window *, input::KeyboardInput)> &callback)
    {
        return m_onKeyboardInput.addListener(callback);
    }
    /** Removes the callback function for keyboard input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnKeyboardInputEventListener(uint32_t callbackID)
    {
        m_onKeyboardInput.removeListener(callbackID);
    }

    /// Creates the actual window from given specification.
    void create();
    /// Updates the window and checks for events. Called every frame.
    void update() const;
    /// Closes the window.
    void close() const;

  public:
    explicit Window(string name = "Window", bool fullscreen = true,
                    bool resizable = false, int width = 1280, int height = 720)
        : m_glfwWindow{nullptr}, m_name{std::move(name)},
          m_fullscreen{fullscreen},
          m_resizable{resizable}, m_width{width}, m_height{height} {};
    ~Window() = default;
    Window(const Window &window) = delete;
    Window &operator=(const Window &window) = delete;

    inline bool operator==(const Window &rhs) const
    {
        return m_glfwWindow == rhs.m_glfwWindow;
    }

    /// The pixel resolution of the framebuffer of the window.
    const array<uint32_t, 2> &resolution = m_resolution;

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

    /** Gets a vector of required extensions when creating an instance of
     * Vulkan.
     *
     * @return std::vector<const char *> A vector containing the names of the
     * required extensions.
     */
    [[nodiscard]] static std::vector<const char *>
    getReqInstanceExtensionsForVulkan();
};
} // namespace kirana::window

#endif