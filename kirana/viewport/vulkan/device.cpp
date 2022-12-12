#include "device.hpp"
#include "instance.hpp"
#include "surface.hpp"

#include "vulkan_utils.hpp"

#include <map>
#include <string>

kirana::viewport::vulkan::QueueFamilyIndices kirana::viewport::vulkan::Device::
    getQueueFamilyIndices(const vk::PhysicalDevice &gpu,
                          const vk::SurfaceKHR &surface)
{
    QueueFamilyIndices indices;

    auto queueFamilies = gpu.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto &q : queueFamilies)
    {
        if (q.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
    }

    vk::Bool32 isPresentSupported =
        gpu.getSurfaceSupportKHR(indices.graphics, surface);
    if (isPresentSupported)
        indices.presentation = indices.graphics;

    return indices;
}

kirana::viewport::vulkan::SwapchainSupportInfo kirana::viewport::vulkan::
    Device::getSwapchainSupportInfo(const vk::PhysicalDevice &gpu,
                                    const vk::SurfaceKHR &surface)
{
    SwapchainSupportInfo supportInfo;
    supportInfo.capabilities = gpu.getSurfaceCapabilitiesKHR(surface);
    supportInfo.surfaceFormats = gpu.getSurfaceFormatsKHR(surface);
    supportInfo.presentModes = gpu.getSurfacePresentModesKHR(surface);

    return supportInfo;
}

bool kirana::viewport::vulkan::Device::selectIdealGPU()
{
    std::vector<vk::PhysicalDevice> devices =
        m_instance->current.enumeratePhysicalDevices();

    // Go through all the devices and rate them based on their properties
    // or features. The devices are stored in a multimap in ascending order
    // of their score.
    std::multimap<uint64_t, vk::PhysicalDevice> deviceScores;
    uint64_t score = 0;
    for (const vk::PhysicalDevice &d : devices)
    {
        if (!d.getFeatures().geometryShader)
        {
            score = 0;
            deviceScores.insert(std::make_pair(score, d));
            continue;
        }
        if (d.getProperties().deviceType ==
            vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
            deviceScores.insert(std::make_pair(score, d));
            break;
        }
        score +=
            static_cast<uint64_t>(d.getProperties().limits.maxImageDimension2D);
        for (const auto &h : d.getMemoryProperties().memoryHeaps)
        {
            if (h.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                score += static_cast<uint64_t>(h.size);
        }
        std::string name = d.getProperties().deviceName.data();
        deviceScores.insert(std::make_pair(score, d));
    }

    // If the last device has a score of 0, then it failed to find any
    // suitable device.
    if (deviceScores.rbegin()->first > 0)
        m_gpu = deviceScores.rbegin()->second;
    else
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to find ideal GPU");
        return false;
    }

    // Check if the device supports necessary features.
    vk::PhysicalDeviceShaderDrawParametersFeatures shaderDrawFeature{};
    vk::PhysicalDeviceFeatures2 features{};
    features.pNext = &shaderDrawFeature;
    m_gpu.getFeatures2(&features);
    if (!features.features.fillModeNonSolid || !features.features.wideLines ||
        !features.features.logicOp || !shaderDrawFeature.shaderDrawParameters)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to find GPU with necessary features");
        return false;
    }

    // Check if the device has the necessary queue families.
    m_queueFamilyIndices = getQueueFamilyIndices(m_gpu, m_surface->current);
    if (!(m_queueFamilyIndices.isGraphicsSupported() &&
          m_queueFamilyIndices.isPresentationSupported()))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to find GPU with necessary Queue Families");
        return false;
    }

    // Check if device has the required extensions.
    if (!hasRequiredExtensions(m_gpu.enumerateDeviceExtensionProperties(),
                               DEVICE_EXTENSIONS))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to find GPU with required device extensions");
        return false;
    }

    // Check if device has swapchain capabilities.
    m_swapchainSupportInfo = getSwapchainSupportInfo(m_gpu, m_surface->current);
    if (!m_swapchainSupportInfo.isSupported())
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to find GPU with swapchain capabilities");
        return false;
    }

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                      "Selected GPU: " +
                          std::string(m_gpu.getProperties().deviceName.data()));
    return true;
}

bool kirana::viewport::vulkan::Device::createLogicalDevice()
{
    std::set<uint32_t> indices = m_queueFamilyIndices.getIndices();

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priority = 1.0f;
    for (auto i : indices)
    {
        vk::DeviceQueueCreateInfo createInfo(vk::DeviceQueueCreateFlags(), i, 1,
                                             &priority);
        queueCreateInfos.push_back(createInfo);
    }

    vk::PhysicalDeviceShaderDrawParametersFeatures shaderDrawFeature{true};
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = true;
    deviceFeatures.wideLines = true;
    deviceFeatures.logicOp = true;
    vk::DeviceCreateInfo createInfo({}, queueCreateInfos, VALIDATION_LAYERS,
                                    DEVICE_EXTENSIONS, &deviceFeatures,
                                    &shaderDrawFeature);

    try
    {
        m_current = m_gpu.createDevice(createInfo);
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_current);
#endif
    }
    catch (...)
    {
        handleVulkanException();
        return false;
    }

    return true;
}

kirana::viewport::vulkan::Device::Device(const Instance *const instance,
                                         const Surface *surface)
    : m_isInitialized{false}, m_instance{instance}, m_surface{surface}
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Finding ideal GPU...");
    if (selectIdealGPU())
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Creating Logical Device for the GPU...");
        if (createLogicalDevice())
        {
            m_graphicsQueue =
                m_current.getQueue(m_queueFamilyIndices.graphics, 0);
            m_presentationQueue =
                m_current.getQueue(m_queueFamilyIndices.presentation, 0);
            m_isInitialized = true;
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Device initialized");
        }
    }
}

kirana::viewport::vulkan::Device::~Device()
{
    if (m_current)
    {
        m_current.destroy();
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Device destroyed");
    }
}


void kirana::viewport::vulkan::Device::reinitializeSwapchainInfo()
{
    m_swapchainSupportInfo = getSwapchainSupportInfo(m_gpu, m_surface->current);
}

void kirana::viewport::vulkan::Device::waitUntilIdle() const
{
    m_current.waitIdle();
}

void kirana::viewport::vulkan::Device::graphicsSubmit(
    const vk::Semaphore &waitSemaphore, vk::PipelineStageFlags stageFlags,
    const vk::CommandBuffer &commandBuffer,
    const vk::Semaphore &signalSemaphore, const vk::Fence &fence) const
{
    m_graphicsQueue.submit(vk::SubmitInfo(waitSemaphore, stageFlags,
                                          commandBuffer, signalSemaphore),
                           fence);
}

void kirana::viewport::vulkan::Device::graphicsSubmit(
    const vk::CommandBuffer &commandBuffer, const vk::Fence &fence) const
{
    m_graphicsQueue.submit(vk::SubmitInfo({}, {}, commandBuffer, {}), fence);
}

vk::Result kirana::viewport::vulkan::Device::present(
    const vk::Semaphore &semaphore, const vk::SwapchainKHR &swapchain,
    uint32_t imageIndex) const
{
    // The following function is written in C API, because C++ function
    // throws an exception for eErrorOutOfDateKHR result value, which is not
    // ideal.
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = (VkSemaphore *)(&semaphore);
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = (VkSwapchainKHR *)(&swapchain);
    presentInfo.pImageIndices = &imageIndex;
    VkResult result = vkQueuePresentKHR(
        static_cast<VkQueue>(m_presentationQueue), &presentInfo);

    return vk::Result(result);
}