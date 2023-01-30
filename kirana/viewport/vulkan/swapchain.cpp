#include "swapchain.hpp"
#include "device.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "vulkan_utils.hpp"

#include <algorithm>
#include <string>

void kirana::viewport::vulkan::Swapchain::initializeSwapchainData()
{
    m_supportInfo = m_device->swapchainSupportInfo;

    // Select surface resolution
    if (m_supportInfo.capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
        m_extent = m_supportInfo.capabilities.currentExtent;
    else
    {
        const auto &resolution = m_surface->getResolution();
        uint32_t width =
            std::clamp(static_cast<uint32_t>(resolution[0]),
                       m_supportInfo.capabilities.minImageExtent.width,
                       m_supportInfo.capabilities.maxImageExtent.width);
        uint32_t height =
            std::clamp(static_cast<uint32_t>(resolution[1]),
                       m_supportInfo.capabilities.minImageExtent.height,
                       m_supportInfo.capabilities.maxImageExtent.height);
        m_extent.setWidth(width);
        m_extent.setHeight(height);
    }

    // Select image count
    m_imageCount = m_supportInfo.capabilities.minImageCount + 1;
    if (m_supportInfo.capabilities.maxImageCount > 0 &&
        m_imageCount > m_supportInfo.capabilities.maxImageCount)
        m_imageCount = m_supportInfo.capabilities.maxImageCount;

    // Get current surface transform
    m_transform = m_supportInfo.capabilities.currentTransform;

    // Select surface format
    auto format = std::find_if(
        m_supportInfo.surfaceFormats.begin(),
        m_supportInfo.surfaceFormats.end(), [](const vk::SurfaceFormatKHR &f) {
            return f.format == vk::Format::eR32G32B32A32Sfloat &&
                   f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
    if (format != m_supportInfo.surfaceFormats.end())
        m_surfaceFormat = *format;
    else
        m_surfaceFormat = m_supportInfo.surfaceFormats[0];

    // Select present mode
    auto presentMode = std::find_if(
        m_supportInfo.presentModes.begin(), m_supportInfo.presentModes.end(),
        [](const vk::PresentModeKHR &p) {
            return p == vk::PresentModeKHR::eMailbox;
        });
    if (presentMode != m_supportInfo.presentModes.end())
        m_presentMode = *presentMode;
    else
        m_presentMode = vk::PresentModeKHR::eFifo;
}

bool kirana::viewport::vulkan::Swapchain::createSwapchain()
{
    vk::SharingMode sharingMode =
        m_device->queueFamilyIndices.isGraphicsAndPresentSame()
            ? vk::SharingMode::eExclusive
            : vk::SharingMode::eConcurrent;

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(), m_surface->current, m_imageCount,
        m_surfaceFormat.format, m_surfaceFormat.colorSpace, m_extent, 1,
        vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransferDst,
        sharingMode, {}, m_transform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
        m_presentMode, true, m_prevSwapchain);

    if (sharingMode == vk::SharingMode::eConcurrent)
    {
        std::vector<uint32_t> indices = std::vector<uint32_t>(
            m_device->queueFamilyIndices.getIndices().begin(),
            m_device->queueFamilyIndices.getIndices().end());
        createInfo.setQueueFamilyIndices(indices);
    }
    try
    {
        m_current = m_device->current.createSwapchainKHR(createInfo);
    }
    catch (...)
    {
        handleVulkanException();
        return false;
    }

    std::vector<vk::Image> swapchainImages =
        m_device->current.getSwapchainImagesKHR(m_current);

    m_images.clear();
    for (uint32_t i = 0; i < swapchainImages.size(); i++)
    {
        const Texture::Properties properties{
            {m_extent.width, m_extent.height, 1},
            m_surfaceFormat.format,
            vk::ImageUsageFlagBits::eColorAttachment |
                vk::ImageUsageFlagBits::eTransferDst,
            vk::ImageAspectFlagBits::eColor,
            false,
            vk::ImageLayout::ePresentSrcKHR};
        m_images.emplace_back(std::move(
            std::make_unique<Texture>(m_device, swapchainImages[i], properties,
                                      "Swapchain_" + std::to_string(i))));
    }
    return true;
}

kirana::viewport::vulkan::Swapchain::Swapchain(const Device *const device,
                                               const Surface *const surface)
    : m_isInitialized{false},
      m_prevSwapchain{nullptr}, m_device{device}, m_surface{surface}
{
    initializeSwapchainData();
    if (createSwapchain())
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Swapchain created");
        m_isInitialized = true;
    }
}

kirana::viewport::vulkan::Swapchain::~Swapchain()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroySwapchainKHR(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Swapchain destroyed");
        }
    }
}

vk::ResultValue<uint32_t> kirana::viewport::vulkan::Swapchain::acquireNextImage(
    uint64_t timeout, const vk::Semaphore &semaphore,
    const vk::Fence &fence) const
{
    // The following function is written in C API, because C++ function
    // throws an exception for eErrorOutOfDateKHR result value, which is not
    // ideal.
    uint32_t imgIndex = 0;
    VkResult result =
        vkAcquireNextImageKHR(static_cast<VkDevice>(m_device->current),
                              static_cast<VkSwapchainKHR>(m_current), timeout,
                              static_cast<VkSemaphore>(semaphore),
                              static_cast<VkFence>(fence), &imgIndex);

    if (vk::Result(result) == vk::Result::eErrorOutOfDateKHR)
        m_onSwapchainOutOfDate();

    return {vk::Result(result), imgIndex};
}
std::array<uint32_t, 2> kirana::viewport::vulkan::Swapchain::
    getSurfaceResolution() const
{
    return m_surface->getResolution();
}