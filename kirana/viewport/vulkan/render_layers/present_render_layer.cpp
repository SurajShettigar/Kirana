#include "present_render_layer.hpp"

#include "../device.hpp"
#include "../swapchain.hpp"
#include "../texture.hpp"
#include "../renderpass.hpp"
#include "../command_buffers.hpp"


kirana::viewport::vulkan::PresentRenderLayer::PresentRenderLayer(
    const Device *device, const Swapchain *swapchain,
    const Texture *depthTexture)
    : RenderLayerBase(device, swapchain, depthTexture)
{
    m_renderPass = new RenderPass(
        m_device,
        RenderPass::Properties{depthTexture != nullptr, false, false,
                               RenderPass::RenderPassType::LAST,
                               m_swapchain->imageFormat,
                               m_depthTexture->getProperties().format});
    m_isInitialized = m_renderPass->isInitialized;
    if (m_isInitialized)
    {
        m_frameBuffers.clear();
        const auto &si = m_swapchain->getImages();

        vk::FramebufferCreateInfo frameBufferInfo(
            vk::FramebufferCreateFlags{}, m_renderPass->current, {},
            m_frameBufferSize[0], m_frameBufferSize[1], 1);
        m_frameBuffers.resize(si.size());
        for (size_t i = 0; i < si.size(); i++)
        {
            std::vector<vk::ImageView> attachments{
                m_swapchain->getImage(i).getImageView(),
                m_depthTexture->getImageView()};
            frameBufferInfo.setAttachments(attachments);
            m_frameBuffers[i] =
                m_device->current.createFramebuffer(frameBufferInfo);
        }
    }
}

bool kirana::viewport::vulkan::PresentRenderLayer::render(
    const CommandBuffers &commandBuffers, uint32_t swapchainIndex)
{
    m_frameBufferSize = m_swapchain->getSurfaceResolution();
    vk::Rect2D rect{{0, 0}, {m_frameBufferSize[0], m_frameBufferSize[1]}};

    commandBuffers.beginRenderPass(m_renderPass->current,
                                   m_frameBuffers.at(swapchainIndex), rect,
                                   {});
    commandBuffers.endRenderPass();
}