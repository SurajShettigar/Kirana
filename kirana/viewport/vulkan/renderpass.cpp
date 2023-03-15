#include "renderpass.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::RenderPass::RenderPass(const Device *const device,
                                                 Properties properties)
    : m_isInitialized{false}, m_device{device}, m_properties{
                                                    std::move(properties)}
{
    std::vector<vk::AttachmentDescription> attachments;

    // Description of the image render pass will be writing into.
    const vk::AttachmentDescription colorAttachmentDesc(
        vk::AttachmentDescriptionFlags(), m_properties.colorFormat,
        vk::SampleCountFlagBits::e1,
        m_properties.clearColor ? vk::AttachmentLoadOp::eClear
                                : vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        m_properties.type == RenderPassType::FIRST
            ? vk::ImageLayout::eUndefined
            : vk::ImageLayout::eColorAttachmentOptimal,
        m_properties.type == RenderPassType::LAST
            ? vk::ImageLayout::ePresentSrcKHR
            : (m_properties.type == RenderPassType::OFFSCREEN
                   ? vk::ImageLayout::eShaderReadOnlyOptimal
                   : vk::ImageLayout::eColorAttachmentOptimal));

    // Description of the depth buffer image for z-testing.
    const vk::AttachmentDescription depthAttachmentDesc(
        vk::AttachmentDescriptionFlags(), m_properties.depthFormat,
        vk::SampleCountFlagBits::e1,
        m_properties.clearDepth ? vk::AttachmentLoadOp::eClear
                                : vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        m_properties.clearDepth
            ? vk::ImageLayout::eUndefined
            : vk::ImageLayout::eDepthStencilAttachmentOptimal,
        m_properties.type == RenderPassType::OFFSCREEN
            ? vk::ImageLayout::eShaderReadOnlyOptimal
            : vk::ImageLayout::eDepthStencilAttachmentOptimal);

    attachments.push_back(colorAttachmentDesc);
    attachments.push_back(depthAttachmentDesc);

    // Create attachment references for sub-passes.
    const vk::AttachmentReference colorAttachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);
    const vk::AttachmentReference depthAttachmentRef(
        1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // Create subpass.
    const vk::SubpassDescription subpassDesc(
        vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {},
        colorAttachmentRef, {}, &depthAttachmentRef);

    // Create subpass dependencies.

    std::vector<vk::SubpassDependency> subpassDependencies;
    if (m_properties.type == RenderPassType::OFFSCREEN)
    {
        const vk::SubpassDependency d1{
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eShaderRead,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::DependencyFlagBits::eByRegion};
        const vk::SubpassDependency d2{
            0,
            VK_SUBPASS_EXTERNAL,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::AccessFlagBits::eShaderRead,
            vk::DependencyFlagBits::eByRegion};
        subpassDependencies.push_back(d1);
        subpassDependencies.push_back(d2);
    }
    else
    {
        const vk::SubpassDependency colorDependency(
            VK_SUBPASS_EXTERNAL, 0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentRead |
                vk::AccessFlagBits::eColorAttachmentWrite);

        const vk::SubpassDependency depthDependency(
            VK_SUBPASS_EXTERNAL, 0,
            vk::PipelineStageFlagBits::eEarlyFragmentTests |
                vk::PipelineStageFlagBits::eLateFragmentTests,
            vk::PipelineStageFlagBits::eEarlyFragmentTests |
                vk::PipelineStageFlagBits::eLateFragmentTests,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eDepthStencilAttachmentRead |
                vk::AccessFlagBits::eDepthStencilAttachmentWrite);
        subpassDependencies.push_back(colorDependency);
        subpassDependencies.push_back(depthDependency);
    }

    // Create Renderpass.
    vk::RenderPassCreateInfo createInfo(vk::RenderPassCreateFlags(),
                                        attachments, subpassDesc,
                                        subpassDependencies);

    try
    {
        m_current = m_device->current.createRenderPass(createInfo);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Renderpass created");
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
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
    }
}