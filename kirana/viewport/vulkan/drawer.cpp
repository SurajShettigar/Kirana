#include "drawer.hpp"

#include "command_pool.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "pipeline.hpp"
#include "scene_data.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

kirana::viewport::vulkan::Drawer::Drawer(const Device *const device,
                                         const Swapchain *const swapchain,
                                         const RenderPass *const renderPass,
                                         const SceneData *const scene)
    : m_isInitialized{false}, m_currentFrameNumber{0}, m_device{device},
      m_swapchain{swapchain}, m_renderPass{renderPass}, m_scene{scene}
{
    m_commandPool =
        new CommandPool(m_device, m_device->queueFamilyIndices.graphics);
    m_mainCommandBuffers = new CommandBuffers(m_device, m_commandPool);
    try
    {
        m_renderFence = m_device->current.createFence(
            vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        m_renderSemaphore =
            m_device->current.createSemaphore(vk::SemaphoreCreateInfo());
        m_presentSemaphore =
            m_device->current.createSemaphore(vk::SemaphoreCreateInfo());

        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Synchronization structures initialized");
        m_isInitialized = true;
    }
    catch (...)
    {
        handleVulkanException();
    }

    m_shader = new Shader(m_device, constants::VULKAN_SHADER_TRIANGLE_NAME);
    if (m_shader->isInitialized)
    {
        std::vector<Shader *> shaders{m_shader};
        m_trianglePipelineLayout = new PipelineLayout(m_device);
        m_trianglePipeline = new Pipeline(
            m_device, m_renderPass, shaders, m_trianglePipelineLayout,
            m_scene != nullptr ? m_scene->vertexDesc : VertexInputDescription(),
            m_swapchain->getWindowResolution());
    }
}

kirana::viewport::vulkan::Drawer::~Drawer()
{
    if (m_device)
    {
        if (m_renderFence)
            VK_HANDLE_RESULT(
                m_device->current.waitForFences(
                    m_renderFence, true, constants::VULKAN_FRAME_SYNC_TIMEOUT),
                "Failed to wait for render fence");

        if (m_trianglePipeline)
        {
            delete m_trianglePipeline;
            m_trianglePipeline = nullptr;
        }
        if (m_trianglePipelineLayout)
        {
            delete m_trianglePipelineLayout;
            m_trianglePipelineLayout = nullptr;
        }
        if (m_shader)
        {
            delete m_shader;
            m_shader = nullptr;
        }
        if (m_renderFence)
            m_device->current.destroyFence(m_renderFence);
        if (m_renderSemaphore)
            m_device->current.destroySemaphore(m_renderSemaphore);
        if (m_presentSemaphore)
            m_device->current.destroySemaphore(m_presentSemaphore);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Synchronization structures destroyed");

        if (m_commandPool)
        {
            delete m_commandPool;
            m_commandPool = nullptr;
        }
        if (m_mainCommandBuffers)
        {
            delete m_mainCommandBuffers;
            m_mainCommandBuffers = nullptr;
        }
    }
}

void kirana::viewport::vulkan::Drawer::draw()
{
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(m_renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence");
    m_device->current.resetFences(m_renderFence);

    uint32_t imgIndex = m_swapchain->acquireNextImage(
        constants::VULKAN_FRAME_SYNC_TIMEOUT, m_presentSemaphore, nullptr);

    vk::ClearValue clearColor;
    std::array<float, 4> color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearColor.setColor(vk::ClearColorValue(color));

    vk::ClearValue clearDepth;
    clearDepth.setDepthStencil(vk::ClearDepthStencilValue(1.0f));

    m_mainCommandBuffers->reset();
    m_mainCommandBuffers->begin();
    m_mainCommandBuffers->beginRenderPass(
        m_renderPass->current, m_renderPass->framebuffers[imgIndex],
        m_swapchain->imageExtent,
        std::vector<vk::ClearValue>{clearColor, clearDepth});
    m_mainCommandBuffers->bindPipeline(m_trianglePipeline->current);

    if (m_scene)
    {
        MeshPushConstants meshConstants;
        // TODO: Bind Vertex Buffers together and draw them at once.
        for (size_t i = 0; i < m_scene->meshes.size(); i++)
        {
            meshConstants.renderMatrix = m_scene->getClipSpaceMatrix(i, 0);

            m_mainCommandBuffers->pushConstants(
                m_trianglePipelineLayout->current,
                vk::ShaderStageFlagBits::eVertex, 0, meshConstants);

            m_mainCommandBuffers->bindVertexBuffer(
                *(m_scene->meshes[i].vertexBuffer.buffer), 0);
            m_mainCommandBuffers->draw(
                static_cast<uint32_t>(m_scene->meshes[i].vertexCount), 1, 0, 0);
        }
    }

    m_mainCommandBuffers->endRenderPass();
    m_mainCommandBuffers->end();

    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsQueue.submit(
        vk::SubmitInfo(m_presentSemaphore, pipelineFlags,
                       m_mainCommandBuffers->current[0], m_renderSemaphore),
        m_renderFence);

    VK_HANDLE_RESULT(m_device->presentationQueue.presentKHR(vk::PresentInfoKHR(
                         m_renderSemaphore, m_swapchain->current, imgIndex)),
                     "Failed to present rendered image");

    m_currentFrameNumber++;
}

/*void kirana::viewport::vulkan::Drawer::loadScene(SceneData *scene)
{
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(m_renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence");
    m_scene = scene;
    m_trianglePipeline->rebuild(m_scene->vertexDesc);
}*/
