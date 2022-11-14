#ifndef PLATFORM_WINDOW_HPP
#define PLATFORM_WINDOW_HPP

#include "window.hpp"

#include <utility>

struct HWND__;
typedef HWND__ *HWND;

namespace kirana::window
{
class PlatformWindow : public Window
{
    friend class WindowManager;

  protected:
    int m_windowPointer = 0;
    HWND m_hwndWindowPointer;

    /// Creates the actual window from given specification.
    void create() override{};
    /// Updates the window and checks for events. Called every frame.
    void update() const override{};
    /// Closes the window.
    void close() const override{};

  public:
    explicit PlatformWindow(int windowPointer, string name = "Window",
                            bool fullscreen = true, bool resizable = false,
                            int width = 1280, int height = 720);
    ~PlatformWindow() override = default;
    PlatformWindow(const PlatformWindow &window) = delete;
    PlatformWindow &operator=(const PlatformWindow &window) = delete;

    inline bool operator==(const PlatformWindow &rhs) const
    {
        return m_windowPointer == rhs.m_windowPointer;
    }

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