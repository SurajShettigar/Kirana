#include "framebuffer.hpp"

#include "device.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::Framebuffer::Framebuffer(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<const Texture *> &colorAttachments,
    const Texture *const depthAttachment, const std::array<uint32_t, 2> &size)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_colorAttachments{colorAttachments},
      m_depthAttachment{depthAttachment}, m_size{size}
{
    std::vector<vk::ImageView> attachments;
    attachments.reserve(depthAttachment != nullptr ? colorAttachments.size() + 1
                                                   : colorAttachments.size());
    for (const auto &t : m_colorAttachments)
        attachments.push_back(t->getImageView());
    if (depthAttachment != nullptr)
        attachments.push_back(depthAttachment->getImageView());

    const vk::FramebufferCreateInfo createInfo{vk::FramebufferCreateFlags{},
                                               m_renderPass->current,
                                               attachments,
                                               m_size[0],
                                               m_size[1],
                                               1};
    try
    {
        m_current = m_device->current.createFramebuffer(createInfo);
        utils::Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, utils::LogSeverity::trace,
            "Framebuffer created for renderpass: " + m_renderPass->name);
        m_isInitialized = true;
    }
    catch (...)
    {
        vulkan::handleVulkanException();
        m_isInitialized = false;
    }
}

kirana::viewport::vulkan::Framebuffer::~Framebuffer()
{
    if (m_current)
    {
        m_device->current.destroyFramebuffer(m_current);
        utils::Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, utils::LogSeverity::trace,
            "Framebuffer destroyed for renderpass: " + m_renderPass->name);
    }
}