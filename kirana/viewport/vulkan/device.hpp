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
    vk::PhysicalDeviceAccelerationStructurePropertiesKHR
        m_accelStructProperties;
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR m_raytracingProperties;

    const Instance *const m_instance;
    const Surface *const m_surface;

    static QueueFamilyIndices getQueueFamilyIndices(
        const vk::PhysicalDevice &gpu, const vk::SurfaceKHR &surface);
    static SwapchainSupportInfo getSwapchainSupportInfo(
        const vk::PhysicalDevice &gpu, const vk::SurfaceKHR &surface);
    static vk::PhysicalDeviceAccelerationStructurePropertiesKHR
    getAccelerationStructureProperties(const vk::PhysicalDevice &gpu);
    static vk::PhysicalDeviceRayTracingPipelinePropertiesKHR
    getRaytracingProperties(const vk::PhysicalDevice &gpu);
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
    const vk::PhysicalDeviceAccelerationStructurePropertiesKHR
        &accelStructProperties = m_accelStructProperties;
    const vk::PhysicalDeviceRayTracingPipelinePropertiesKHR
        &raytracingProperties = m_raytracingProperties;

    inline vk::DeviceSize alignSize(vk::DeviceSize size,
                                    vk::DeviceSize alignment) const
    {
        return alignment > 0 ? (size + alignment - 1) & ~(alignment - 1) : size;
    }

    inline uint32_t alignSize(uint32_t size, uint32_t alignment) const
    {
        return alignment > 0 ? (size + alignment - 1) & ~(alignment - 1) : size;
    }

    inline vk::DeviceSize alignUniformBufferSize(vk::DeviceSize size) const
    {
        return alignSize(
            size, m_gpu.getProperties().limits.minUniformBufferOffsetAlignment);
    }

    void reinitializeSwapchainInfo();
    void waitUntilIdle() const;
    void graphicsSubmit(const vk::Semaphore &waitSemaphore,
                        vk::PipelineStageFlags stageFlags,
                        const vk::CommandBuffer &commandBuffer,
                        const vk::Semaphore &signalSemaphore,
                        const vk::Fence &fence) const;
    void graphicsSubmit(const vk::CommandBuffer &commandBuffer,
                        const vk::Fence &fence) const;
    void graphicsSubmit(
        const std::vector<vk::CommandBuffer> &commandBuffers) const;
    void graphicsWait() const;
    [[nodiscard]] vk::Result present(const vk::Semaphore &semaphore,
                                     const vk::SwapchainKHR &swapchain,
                                     uint32_t imageIndex) const;
    [[nodiscard]] vk::DeviceAddress getBufferAddress(
        const vk::Buffer &buffer) const;

    void setDebugObjectName(vk::ObjectType type, uint64_t handle,
                            const std::string &name) const;
    void setDebugObjectName(const vk::Buffer &buffer,
                            const std::string &name) const;
    void setDebugObjectName(const vk::ShaderModule &shaderModule,
                            const std::string &name) const;
    void setDebugObjectName(const vk::Pipeline &pipeline,
                            const std::string &name) const;
    void setDebugObjectName(const vk::AccelerationStructureKHR &accelStruct,
                            const std::string &name) const;
};
} // namespace kirana::viewport::vulkan


#endif