#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Surface;
class Texture;

class Swapchain
{
  private:
    utils::Event<> m_onSwapchainOutOfDate;

    bool m_isInitialized = false;

    SwapchainSupportInfo m_supportInfo;
    vk::SurfaceFormatKHR m_surfaceFormat;
    uint32_t m_imageCount;
    vk::SurfaceTransformFlagBitsKHR m_transform;
    vk::PresentModeKHR m_presentMode;
    vk::Extent2D m_extent;

    vk::SwapchainKHR m_prevSwapchain = nullptr;
    vk::SwapchainKHR m_current = nullptr;
    std::vector<const Texture *> m_images;

    const Device *const m_device = nullptr;
    const Surface *const m_surface;

    /// Initializes swapchain data like surface format, present mode etc.
    void initializeSwapchainData();
    /// Creates swapchain object.
    bool createSwapchain();

  public:
    Swapchain(const Device *device, const Surface *surface);
    ~Swapchain();
    Swapchain(const Swapchain &swapchain) = delete;
    Swapchain &operator=(const Swapchain &swapchain) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::SwapchainKHR &current = m_current;
    const vk::Format &imageFormat = m_surfaceFormat.format;
    const vk::Extent2D &imageExtent = m_extent;

    bool initialize();

    inline uint32_t addOnSwapchainOutOfDateListener(
        const std::function<void()> &callback)
    {
        return m_onSwapchainOutOfDate.addListener(callback);
    }
    inline void removeOnSwapchainOutOfDateListener(uint32_t callbackID)
    {
        return m_onSwapchainOutOfDate.removeListener(callbackID);
    }

    [[nodiscard]] vk::ResultValue<uint32_t> acquireNextImage(
        uint64_t timeout = 1000000000, const vk::Semaphore &semaphore = {},
        const vk::Fence &fence = {}) const;
    [[nodiscard]] std::array<uint32_t, 2> getSurfaceResolution() const;

    inline const std::vector<const Texture *> &getImages() const
    {
        return m_images;
    }

    [[nodiscard]] inline const Texture &getImage(uint32_t index) const
    {
        return *m_images[index];
    }
};
} // namespace kirana::viewport::vulkan

#endif // SWAPCHAIN_HPP
