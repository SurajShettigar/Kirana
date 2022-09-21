#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <GLFW/glfw3.h>

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <event.hpp>

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
    utils::Event<Window *> m_onWindowCloseEvent;
    /**
     * @brief Called by GLFW when close flag of a window is set
     *
     * @param glfwWindow The window which is being currently closed
     */
    static void onWindowClosed(GLFWwindow *glfwWindow);

    /**
     * @brief Adds the callback function which is called when a window is
     * closed.
     *
     * @param callback The function which will be called.
     * @return uint32_t
     */
    inline uint32_t addOnWindowCloseListener(
        const std::function<void(Window *)> &callback)
    {
        return m_onWindowCloseEvent.addListener(callback);
    }
    /**
     * @brief Removes the callback function with given identifier from being
     * called after the event.
     *
     * @param callbackID
     */
    inline void removeOnWindowCloseListener(uint32_t callbackID)
    {
        m_onWindowCloseEvent.removeListener(callbackID);
    }
    /// Removes all the callback functions for the window close event.
    inline void removeAllOnWindowCloseListener()
    {
        m_onWindowCloseEvent.removeAllListeners();
    }

    /// Creates the actual window from given specification.
    void create();
    /// Updates the window and checks for events. Called every frame.
    void update() const;
    /// Closes the window.
    void close() const;

  public:
    string name = "Window";
    int width = 1280;
    int height = 720;

    explicit Window(string name = "Window", int width = 1280, int height = 720)
        : m_glfwWindow{nullptr}, name{std::move(name)}, width{width},
          height{height} {};
    ~Window() = default;
    Window(const Window &window) = delete;
    Window &operator=(const Window &window) = delete;

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
    [[nodiscard]] array<int, 2> getWindowResolution() const;

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