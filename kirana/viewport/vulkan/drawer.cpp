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
#include <vector3.hpp>

kirana::math::Matrix4x4 getTransform(const kirana::camera::Camera &cam,
                                     kirana::math::Transform *model,
                                     float frameNum, bool rotate = false)
{
    if (rotate)
        model->rotateY(1.0f);
    kirana::math::Matrix4x4 mat = kirana::math::Matrix4x4::transpose(
        cam.projection.getMatrix() * cam.transform.getMatrix() *
        model->getMatrix());
    return mat;
}

kirana::viewport::vulkan::Drawer::Drawer(const Device *const device,
                                         const Swapchain *const swapchain,
                                         const RenderPass *const renderPass,
                                         const SceneData *const scene)
    : m_isInitialized{false}, m_currentFrameNumber{0}, m_device{device},
      m_swapchain{swapchain}, m_renderPass{renderPass}, m_scene{scene},
      /*m_camera{camera::OrthographicCamera({1280, 720}, 5.0f, 0.1f, 200.0f,
         true, true)}*/
      m_camera{camera::PerspectiveCamera({1280, 720}, 50.0f, 0.1f, 200.0f, true,
                                         true)}
{
    m_camera.transform.translate(kirana::math::Vector3(0.0f, 1.5f, 3.0f));
    m_camera.transform.lookAt(kirana::math::Vector3::ZERO,
                              kirana::math::Vector3::UP);
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
            m_scene->vertexDesc, m_swapchain->getWindowResolution());
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
        // TODO: Bind Vertex Buffers together and draw them at once.
        for (size_t i = 0; i < m_scene->meshes.size(); i++)
        {
            MeshPushConstants meshConstants;
            meshConstants.renderMatrix =
                getTransform(m_camera, m_scene->meshes[i].instanceTransforms[0],
                             static_cast<float>(m_currentFrameNumber), i == 0);

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
