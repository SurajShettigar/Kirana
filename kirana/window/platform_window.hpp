#ifndef PLATFORM_WINDOW_HPP
#define PLATFORM_WINDOW_HPP

#include "window.hpp"

// Forward declarations of windows.h API
#if defined(_WIN64)
typedef unsigned __int64 UINT_PTR;
typedef __int64 LONG_PTR;
#else
typedef _W64 unsigned int UINT_PTR;
typedef _W64 long LONG_PTR;
#endif
typedef unsigned int UINT;

#define CALLBACK __stdcall

struct HWND__;
typedef HWND__ *HWND;
typedef LONG_PTR LRESULT;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LRESULT(CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

namespace kirana::window
{
class PlatformWindow : public Window
{
    friend class WindowManager;

  protected:
    long m_windowPointer = 0;
    HWND m_hwndWindowPointer = nullptr;
    WNDPROC m_prevWindowProc = nullptr;

    utils::input::ModifierKey getMouseModifierKey(WPARAM wParam);

    void handleWindowEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void handleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void handleKeyboardInput(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

    /// Creates the actual window from given specification.
    void create() override{};
    /// Updates the window and checks for events. Called every frame.
    void update() override;
    /// Closes the window.
    void close() override;

  public:
    explicit PlatformWindow(long windowPointer, string name = "Window",
                            bool fullscreen = true, bool resizable = false,
                            int width = 1280, int height = 720);
    ~PlatformWindow() override = default;
    PlatformWindow(const PlatformWindow &window) = delete;
    PlatformWindow &operator=(const PlatformWindow &window) = delete;

    inline bool operator==(const PlatformWindow &rhs) const
    {
        return m_windowPointer == rhs.m_windowPointer;
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
    [[nodiscard]] std::vector<const char *> getReqInstanceExtensionsForVulkan()
        const override;
};
} // namespace kirana::window

#endif