#include "scene_render_layer.hpp"

#include "../device.hpp"
#include "../swapchain.hpp"
#include "../texture.hpp"
#include "../scene_data.hpp"
#include "../descriptor_pool.hpp"
#include "../descriptor_set_layout.hpp"
#include "../descriptor_set.hpp"
#include "../pipeline_layout.hpp"
#include "../renderpass.hpp"
#include "../command_buffers.hpp"

kirana::viewport::vulkan::SceneRenderLayer::SceneRenderLayer(
    const Device *device, const Swapchain *swapchain,
    const Texture *depthTexture, const SceneData *const sceneData,
    const DescriptorPool *descriptorPool)
    : RenderLayerBase(device, swapchain, depthTexture), m_sceneData{sceneData},
      m_descriptorPool{descriptorPool}
{
    m_renderPass = new RenderPass(
        m_device,
        RenderPass::Properties{depthTexture != nullptr, false, false,
                               RenderPass::RenderPassType::OFFSCREEN,
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

    m_isInitialized = PipelineLayout::getDefaultPipelineLayout(
        m_device, vulkan::ShadingPipeline::RASTER, m_pipelineLayout);
    const auto &descSetLayouts = m_pipelineLayout->getDescriptorSetLayouts();
    m_descSets.clear();
    m_descSets.resize(descSetLayouts.size());
    m_descriptorPool->allocateDescriptorSets(descSetLayouts, &m_descSets);
}

bool kirana::viewport::vulkan::SceneRenderLayer::render(
    const CommandBuffers &commandBuffers, uint32_t swapchainIndex)
{
    m_frameBufferSize = m_swapchain->getSurfaceResolution();
    vk::Rect2D rect{{0, 0}, {m_frameBufferSize[0], m_frameBufferSize[1]}};
    vk::Viewport viewport{0.0f,
                          0.0f,
                          static_cast<float>(m_frameBufferSize[0]),
                          static_cast<float>(m_frameBufferSize[1]),
                          0.0f,
                          1.0f};

    commandBuffers.beginRenderPass(m_renderPass->current,
                                   m_frameBuffers.at(swapchainIndex), rect, {});
    commandBuffers.setViewportScissor(viewport, rect);
    commandBuffers.endRenderPass();
}