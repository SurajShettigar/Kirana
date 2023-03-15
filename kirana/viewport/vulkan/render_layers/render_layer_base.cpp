#include "render_layer_base.hpp"

#include "../device.hpp"
#include "../swapchain.hpp"
#include "../renderpass.hpp"

#include "../vulkan_utils.hpp"

kirana::viewport::vulkan::RenderLayerBase::RenderLayerBase(
    const Device *const device, const Swapchain *const swapchain,
    const Texture *const depthTexture)
    : m_isInitialized{false}, m_device{device}, m_swapchain{swapchain},
      m_depthTexture{depthTexture}, m_frameBufferSize{
                                        m_swapchain->getSurfaceResolution()}
{
    m_renderPass = new RenderPass(m_device, RenderPass::Properties{
                                                m_depthTexture != nullptr,
                                            });
    vk::FramebufferCreateInfo frameBufferInfo(
        vk::FramebufferCreateFlags(), m_current, {},
        m_swapchain->imageExtent.width, m_swapchain->imageExtent.height, 1);

    for (const auto &i : m_swapchain->getImages())
    {
        std::vector<vk::ImageView> framebufferAttachments{
            i->getImageView(), m_depthTexture->getImageView()};
        frameBufferInfo.setAttachments(framebufferAttachments);
        m_frameBuffers.emplace_back(
            m_device->current.createFramebuffer(frameBufferInfo));
    }
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Framebuffers created");
}

kirana::viewport::vulkan::RenderLayerBase::~RenderLayerBase()
{
    if (!m_frameBuffers.empty())
    {
        for (const auto &f : m_frameBuffers)
            m_device->current.destroyFramebuffer(f);
        m_frameBuffers.clear();
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Framebuffers destroyed");
    }
    if (m_renderPass)
    {
        delete m_renderPass;
        m_renderPass = nullptr;
    }
}