#include "renderpass.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "depth_buffer.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::RenderPass::RenderPass(
    const Device *const device, const Swapchain *const swapchain,
    const DepthBuffer *const depthBuffer)
    : m_isInitialized{false}, m_device{device}, m_swapchain{swapchain},
      m_depthBuffer{depthBuffer}
{
    std::vector<vk::AttachmentDescription> attachments;

    // Description of the image render pass will be writing into.
    vk::AttachmentDescription colorAttachmentDesc(
        vk::AttachmentDescriptionFlags(), m_swapchain->imageFormat,
        vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);

    // Description of the depth buffer image for z-testing.
    vk::AttachmentDescription depthAttachmentDesc(
        vk::AttachmentDescriptionFlags(), m_depthBuffer->format,
        vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal);

    attachments.push_back(colorAttachmentDesc);
    attachments.push_back(depthAttachmentDesc);

    // Create attachment references for sub-passes.
    vk::AttachmentReference colorAttachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthAttachmentRef(
        1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // Create subpass.
    vk::SubpassDescription subpassDesc(
        vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {},
        colorAttachmentRef, {}, &depthAttachmentRef);

    // Create subpass dependencies.
    vk::SubpassDependency colorDependency(
        VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eNone, vk::AccessFlagBits::eColorAttachmentWrite);

    vk::SubpassDependency depthDependency(
        VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eEarlyFragmentTests |
            vk::PipelineStageFlagBits::eLateFragmentTests,
        vk::PipelineStageFlagBits::eEarlyFragmentTests |
            vk::PipelineStageFlagBits::eLateFragmentTests,
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    std::vector<vk::SubpassDependency> subpassDependencies;

    // Create Renderpass.
    vk::RenderPassCreateInfo createInfo(vk::RenderPassCreateFlags(),
                                        attachments, subpassDesc,
                                        subpassDependencies);

    try
    {
        m_current = device->current.createRenderPass(createInfo);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Renderpass created");

        vk::FramebufferCreateInfo frameBufferInfo(
            vk::FramebufferCreateFlags(), m_current, {},
            m_swapchain->imageExtent.width, m_swapchain->imageExtent.height, 1);

        m_framebuffers.clear();

        for (const auto &i : m_swapchain->imageViews)
        {
            std::vector<vk::ImageView> framebufferAttachments{
                i, m_depthBuffer->imageView};
            frameBufferInfo.setAttachments(framebufferAttachments);
            m_framebuffers.emplace_back(
                device->current.createFramebuffer(frameBufferInfo));
        }
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
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
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Renderpass destroyed");
        }
        if (!m_framebuffers.empty())
        {
            for (const auto &f : m_framebuffers)
                m_device->current.destroyFramebuffer(f);
            m_framebuffers.clear();
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Framebuffers destroyed");
        }
    }
}

[[nodiscard]] std::array<uint32_t, 2> kirana::viewport::vulkan::RenderPass::
    getSurfaceResolution() const
{
    return m_swapchain->getSurfaceResolution();
}