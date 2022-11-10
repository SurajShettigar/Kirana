#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <input_manager.hpp>
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

using utils::Event;
using utils::input::KeyAction;
using utils::input::Key;
using utils::input::KeyboardInput;
using utils::input::MouseButton;
using utils::input::MouseInput;

class Window
{
    friend class WindowManager;

  protected:
    mutable Event<const Window *, std::array<uint32_t, 2>> m_onWindowResize;
    mutable Event<const Window *> m_onWindowClose;
    mutable Event<const Window *, KeyboardInput> m_onKeyboardInput;
    mutable Event<const Window *, MouseInput> m_onMouseInput;
    mutable Event<const Window *, double, double> m_onScrollInput;

    string m_name = "Window";
    bool m_fullscreen = true;
    bool m_resizable = false;
    int m_width = 1280;
    int m_height = 720;
    std::array<uint32_t, 2> m_resolution{static_cast<uint32_t>(m_width),
                                         static_cast<uint32_t>(m_height)};
    /**
     *@brief Adds the callback function which is called when a window is
     * resized.
     * @param callback The function which will be called.
     * @return uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnWindowResizeListener(
        const std::function<void(const Window *, std::array<uint32_t, 2>)> &callback)
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
        const std::function<void(const Window *)> &callback)
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
        const std::function<void(const Window *, KeyboardInput)> &callback)
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
    /** Adds a callback function for mouse input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnMouseInputEventListener(
        const std::function<void(const Window *, MouseInput)> &callback)
    {
        return m_onMouseInput.addListener(callback);
    }
    /** Removes the callback function for mouse input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnMouseInputEventListener(uint32_t callbackID)
    {
        m_onMouseInput.removeListener(callbackID);
    }
    /** Adds a callback function for scroll input.
     *
     * @param callback The function to be called on input.
     * uint32_t Unique identifier for the callback function. Use this id
     * later to remove the callback function from being called.
     */
    inline uint32_t addOnScrollInputEventListener(
        const std::function<void(const Window *, double, double)> &callback)
    {
        return m_onScrollInput.addListener(callback);
    }
    /** Removes the callback function for scroll input with given identifier
     * from being called after the event.
     *
     * @param callbackID
     */
    inline void removeOnScrollInputEventListener(uint32_t callbackID)
    {
        m_onScrollInput.removeListener(callbackID);
    }

    /// Creates the actual window from given specification.
    virtual void create() = 0;
    /// Updates the window and checks for events. Called every frame.
    virtual void update() const = 0;
    /// Closes the window.
    virtual void close() const = 0;
    /// Cleans the window.
    virtual void clean() const
    {
        m_onWindowResize.removeAllListeners();
        m_onWindowClose.removeAllListeners();
        m_onKeyboardInput.removeAllListeners();
        m_onMouseInput.removeAllListeners();
        m_onScrollInput.removeAllListeners();
    }
  public:
    explicit Window(string name = "Window", bool fullscreen = true,
                    bool resizable = false, int width = 1280, int height = 720)
        : m_name{std::move(name)}, m_fullscreen{fullscreen},
          m_resizable{resizable}, m_width{width}, m_height{height} {};
    virtual ~Window() = default;
    Window(const Window &window) = delete;
    Window &operator=(const Window &window) = delete;

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
    virtual VkResult getVulkanWindowSurface(
        VkInstance instance, const VkAllocationCallbacks *allocator,
        VkSurfaceKHR *surface) const = 0;

    /** Gets a vector of required extensions when creating an instance of
     * Vulkan.
     *
     * @return std::vector<const char *> A vector containing the names of the
     * required extensions.
     */
    [[nodiscard]] virtual std::vector<const char *>
    getReqInstanceExtensionsForVulkan() const = 0;
};
} // namespace kirana::window

#endif