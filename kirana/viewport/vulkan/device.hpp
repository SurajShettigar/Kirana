#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "vulkan_utils.hpp"

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

    QueueFamilyIndices getQueueFamilyIndices(const vk::PhysicalDevice &gpu,
                                             const vk::SurfaceKHR &surface);
    SwapchainSupportInfo getSwapchainSupportInfo(const vk::PhysicalDevice &gpu,
                                                 const vk::SurfaceKHR &surface);
    bool selectIdealGPU();
    bool createLogicalDevice();

  public:
    Device(const Instance *instance, const Surface *surface);
    ~Device();
    Device(const Device &device) = delete;
    Device &operator=(const Device &device) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Device &current = m_current;
    const QueueFamilyIndices &queueFamilyIndices = m_queueFamilyIndices;
    const SwapchainSupportInfo &swapchainSupportInfo = m_swapchainSupportInfo;
    const vk::Queue &graphicsQueue = m_graphicsQueue;
    const vk::Queue &presentationQueue = m_presentationQueue;
};
} // namespace kirana::viewport::vulkan


#endif