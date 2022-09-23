#include "renderpass.hpp"

#include "device.hpp"
#include "swapchain.hpp"

kirana::viewport::vulkan::RenderPass::RenderPass(
    const Device *const device, const Swapchain *const swapchain)
    : m_isInitialized{false}, m_device{device}, m_swapchain{swapchain}
{
    std::vector<vk::AttachmentDescription> attachments;

    // Description of the image render pass will be writing into.
    vk::AttachmentDescription colorAttachmentDesc(
        vk::AttachmentDescriptionFlags(), m_swapchain->imageFormat,
        vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);

    // TODO: Create Depth attachment

    attachments.push_back(colorAttachmentDesc);

    // Create attachment references for sub-passes.
    vk::AttachmentReference colorAttachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);

    // Create subpass.
    vk::SubpassDescription subpassDesc(vk::SubpassDescriptionFlags(),
                                       vk::PipelineBindPoint::eGraphics, {},
                                       colorAttachmentRef);

    // Create Renderpass.
    vk::RenderPassCreateInfo createInfo(vk::RenderPassCreateFlags(),
                                        attachments, subpassDesc);

    try
    {
        m_current = device->current.createRenderPass(createInfo);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Renderpass created");

        vk::FramebufferCreateInfo frameBufferInfo(
            vk::FramebufferCreateFlags(), m_current, {},
            m_swapchain->imageExtent.width, m_swapchain->imageExtent.height, 1);

        m_framebuffers.clear();
        for (const auto &i : m_swapchain->imageViews)
        {
            frameBufferInfo.setAttachments(i);
            m_framebuffers.emplace_back(
                device->current.createFramebuffer(frameBufferInfo));
        }
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Framebuffers created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::RenderPass::~RenderPass()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyRenderPass(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                              "Renderpass destroyed");
        }
        if (!m_framebuffers.empty())
        {
            for (const auto &f : m_framebuffers)
                m_device->current.destroyFramebuffer(f);
            m_framebuffers.clear();
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                              "Framebuffers destroyed");
        }
    }
}