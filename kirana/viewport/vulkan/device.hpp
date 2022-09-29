#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Instance;
class Surface;

class Device
{
  private:
    bool m_isInitialized = false;
    vk::PhysicalDevice m_gpu;
    vk::Device m_current;
    QueueFamilyIndices m_queueFamilyIndices;
    SwapchainSupportInfo m_swapchainSupportInfo;
    vk::Queue m_graphicsQueue;
    vk::Queue m_presentationQueue;

    const Instance *const m_instance;
    const Surface *const m_surface;

    static QueueFamilyIndices getQueueFamilyIndices(
        const vk::PhysicalDevice &gpu, const vk::SurfaceKHR &surface);
    static SwapchainSupportInfo getSwapchainSupportInfo(
        const vk::PhysicalDevice &gpu, const vk::SurfaceKHR &surface);
    /**
     * Selects a GPU based on its capabilities. GPU with the most features is
     * selected.
     * @return true if GPU with at least graphics and presentation capabilities
     * is found.
     */
    bool selectIdealGPU();
    /**
     * Creates a logical device based on the selected GPU.
     * @return true if logical device is successfully created.
     */
    bool createLogicalDevice();

  public:
    Device(const Instance *instance, const Surface *surface);
    ~Device();
    Device(const Device &device) = delete;
    Device &operator=(const Device &device) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::PhysicalDevice &gpu = m_gpu;
    const vk::Device &current = m_current;
    const QueueFamilyIndices &queueFamilyIndices = m_queueFamilyIndices;
    const SwapchainSupportInfo &swapchainSupportInfo = m_swapchainSupportInfo;
    const vk::Queue &graphicsQueue = m_graphicsQueue;
    const vk::Queue &presentationQueue = m_presentationQueue;
};
} // namespace kirana::viewport::vulkan


#endif