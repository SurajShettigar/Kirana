#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Surface;

class Swapchain
{
  private:
    bool m_isInitialized = false;

    SwapchainSupportInfo m_supportInfo;
    vk::SurfaceFormatKHR m_surfaceFormat;
    uint32_t m_imageCount;
    vk::SurfaceTransformFlagBitsKHR m_transform;
    vk::PresentModeKHR m_presentMode;
    vk::Extent2D m_extent;

    vk::SwapchainKHR m_prevSwapchain = nullptr;
    vk::SwapchainKHR m_current;
    std::vector<vk::Image> m_images;
    std::vector<vk::ImageView> m_imageViews;

    const Device *const m_device;
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
    const std::vector<vk::ImageView> &imageViews = m_imageViews;

    uint32_t acquireNextImage(uint64_t timeout = 1000000000,
                              const vk::Semaphore &semaphore = {},
                              const vk::Fence &fence = {}) const;
};
} // namespace kirana::viewport::vulkan

#endif // SWAPCHAIN_HPP
