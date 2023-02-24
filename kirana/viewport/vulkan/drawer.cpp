#include "drawer.hpp"

#include "command_pool.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "descriptor_set.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "pipeline_layout.hpp"
#include "raytrace_pipeline.hpp"
#include "scene_data.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"
#include "raytrace_data.hpp"
#include "texture.hpp"

#include <scene.hpp>
#include <constants.h>


const kirana::viewport::vulkan::FrameData &kirana::viewport::vulkan::Drawer::
    getCurrentFrame() const
{
    return m_frames[m_currentFrameNumber %
                    utils::constants::VULKAN_FRAME_OVERLAP_COUNT];
}

uint32_t kirana::viewport::vulkan::Drawer::getCurrentFrameIndex() const
{
    return m_currentFrameNumber % utils::constants::VULKAN_FRAME_OVERLAP_COUNT;
}

kirana::viewport::vulkan::Drawer::Drawer(const Device *const device,
                                         const Swapchain *const swapchain,
                                         const RenderPass *const renderPass,
                                         const SceneData *const scene)
    : m_isInitialized{false}, m_currentFrameNumber{0}, m_device{device},
      m_swapchain{swapchain}, m_renderPass{renderPass}, m_scene{scene}
{
    m_frames.resize(utils::constants::VULKAN_FRAME_OVERLAP_COUNT);
    for (size_t i = 0; i < utils::constants::VULKAN_FRAME_OVERLAP_COUNT; i++)
    {
        try
        {
            m_frames[i].renderFence = m_device->current.createFence(
                vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
            m_frames[i].renderSemaphore =
                m_device->current.createSemaphore(vk::SemaphoreCreateInfo());
            m_frames[i].presentSemaphore =
                m_device->current.createSemaphore(vk::SemaphoreCreateInfo());

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Synchronization structures initialized");

            m_frames[i].commandPool = new CommandPool(
                m_device, m_device->queueFamilyIndices.graphics);
            m_frames[i].commandPool->allocateCommandBuffers(
                m_frames[i].commandBuffers);
            m_isInitialized = true;
        }
        catch (...)
        {
            handleVulkanException();
        }
    }
    m_onSceneDataChangeListener = m_scene->addOnSceneDataChangeListener(
        [&]() { m_currentFrameNumber = 0; });
}

kirana::viewport::vulkan::Drawer::~Drawer()
{
    m_scene->removeOnSceneDataChangeListener(m_onSceneDataChangeListener);
    if (m_device)
    {
        if (!m_frames.empty())
        {
            for (const auto &f : m_frames)
            {
                if (f.renderFence)
                    VK_HANDLE_RESULT(m_device->current.waitForFences(
                                         f.renderFence, true,
                                         constants::VULKAN_FRAME_SYNC_TIMEOUT),
                                     "Failed to wait for render fence");
                if (f.renderFence)
                    m_device->current.destroyFence(f.renderFence);
                if (f.renderSemaphore)
                    m_device->current.destroySemaphore(f.renderSemaphore);
                if (f.presentSemaphore)
                    m_device->current.destroySemaphore(f.presentSemaphore);

                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::trace,
                                  "Synchronization structures destroyed");

                if (f.commandPool)
                {
                    delete f.commandPool;
                }
                if (f.commandBuffers)
                {
                    delete f.commandBuffers;
                }
            }
        }
    }
}


void kirana::viewport::vulkan::Drawer::rasterizeMeshes(const FrameData &frame,
                                                       bool drawEditorMeshes)
{
    const auto &rPipelineLayout = m_scene->getRasterPipelineLayout().current;
    const auto &meshObjects = drawEditorMeshes ? m_scene->getEditorMeshes()
                                               : m_scene->getSceneMeshes();

    std::string lastPipeline = "";
    int lastVertexBufferIndex = -1;
    int lastIndexBufferIndex = -1;
    for (const auto &mObj : meshObjects)
    {
        for (int mIndex = 0; mIndex < mObj.meshes.size(); mIndex++)
        {
            const auto &mesh = mObj.meshes[mIndex];
            if (mesh.vertexBufferIndex > -1 &&
                mesh.vertexBufferIndex != lastVertexBufferIndex)
            {
                frame.commandBuffers->bindVertexBuffer(
                    m_scene->getVertexBuffer(drawEditorMeshes, mObj.index,
                                             mesh.index),
                    0);
                lastVertexBufferIndex = mesh.vertexBufferIndex;
            }
            if (mesh.indexBufferIndex > -1 &&
                mesh.indexBufferIndex != lastIndexBufferIndex)
            {
                frame.commandBuffers->bindIndexBuffer(
                    m_scene->getIndexBuffer(drawEditorMeshes, mObj.index,
                                            mesh.index),
                    0);
                lastIndexBufferIndex = mesh.indexBufferIndex;
            }
            const auto &pipeline = m_scene->getCurrentPipeline(
                drawEditorMeshes, false, mObj.index, mesh.index);
            if (lastPipeline != pipeline.name)
            {
                frame.commandBuffers->bindPipeline(pipeline.current);
                lastPipeline = pipeline.name;
            }

            for (uint32_t i = 0; i < mObj.instances.size(); i++)
            {
                const auto &pushConstantData =
                    m_scene->getPushConstantRasterData(
                        drawEditorMeshes, false, mObj.index, mesh.index, i);
                frame.commandBuffers->pushConstants<PushConstantRaster>(
                    rPipelineLayout, pushConstantData);

                frame.commandBuffers->drawIndexed(
                    mesh.indexCount, 1, mesh.firstIndex, mesh.vertexOffset,
                    mObj.getGlobalInstanceIndex(i));

                // TODO: Find better way to render outline
                if (*mObj.instances[0].selected)
                {
                    const auto &outline = m_scene->getCurrentPipeline(
                        drawEditorMeshes, true, mObj.index, mesh.index);
                    frame.commandBuffers->bindPipeline(outline.current);
                    lastPipeline = outline.name;

                    const auto &outlinePC = m_scene->getPushConstantRasterData(
                        drawEditorMeshes, true, mObj.index, mesh.index, i);
                    frame.commandBuffers->pushConstants<PushConstantRaster>(
                        rPipelineLayout, outlinePC);

                    frame.commandBuffers->drawIndexed(
                        mesh.indexCount, 1, mesh.firstIndex, mesh.vertexOffset,
                        mObj.getGlobalInstanceIndex(i));
                }
            }
        }
    }
}


void kirana::viewport::vulkan::Drawer::rasterize(const FrameData &frame,
                                                 uint32_t swapchainImgIndex)
{
    const auto &rPipelineLayout = m_scene->getRasterPipelineLayout().current;
    const auto &rDescSets = m_scene->getRasterDescriptorSets();
    std::vector<vk::DescriptorSet> descSets(rDescSets.size());
    for (int i = 0; i < rDescSets.size(); i++)
        descSets[i] = rDescSets[i].current;

    vk::ClearValue clearColor;
    const math::Vector4 ambientColor = m_scene->getWorldData().ambientColor;
    std::array<float, 4> color = {ambientColor[0], ambientColor[1],
                                  ambientColor[2], ambientColor[3]};
    clearColor.setColor(vk::ClearColorValue(color));

    vk::ClearValue clearDepth;
    clearDepth.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0.0));

    frame.commandBuffers->reset();
    frame.commandBuffers->begin();

    frame.commandBuffers->beginRenderPass(
        m_renderPass->current, m_renderPass->framebuffers[swapchainImgIndex],
        m_swapchain->imageExtent,
        std::vector<vk::ClearValue>{clearColor, clearDepth});

    const auto &size = m_swapchain->getSurfaceResolution();
    const vk::Viewport viewport{
        0.0f, 0.0f, static_cast<float>(size[0]), static_cast<float>(size[1]),
        0.0f, 1.0f};
    const vk::Rect2D scissor{{0, 0}, {size[0], size[1]}};
    frame.commandBuffers->setViewportScissor(viewport, scissor);

    frame.commandBuffers->bindDescriptorSets(rPipelineLayout, descSets, {0, 0});

    rasterizeMeshes(frame, true);
    rasterizeMeshes(frame, false);

    frame.commandBuffers->endRenderPass();
    frame.commandBuffers->end();
    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsSubmit(frame.presentSemaphore, pipelineFlags,
                             frame.commandBuffers->current[0],
                             frame.renderSemaphore, frame.renderFence);
}

void kirana::viewport::vulkan::Drawer::raytrace(const FrameData &frame,
                                                uint32_t swapchainImgIndex)
{
    const auto &rPipeline =
        m_scene->getCurrentPipeline(false, false, 0, 0).current;
    const auto &rPipelineLayout =
        m_scene->getRaytraceData().getRaytracePipelineLayout().current;
    const auto &rDescSets = m_scene->getRaytraceData().getDescriptorSets();
    std::vector<vk::DescriptorSet> descSets(rDescSets.size());
    for (int i = 0; i < rDescSets.size(); i++)
        descSets[i] = rDescSets[i].current;
    auto pushConstantData = m_scene->getPushConstantRaytraceData();
    const auto &sbt = m_scene->getCurrentSBT(0, 0);
    const auto &renderTarget = m_scene->getRaytraceData().getRenderTarget();

    frame.commandBuffers->reset();
    frame.commandBuffers->begin();

    frame.commandBuffers->bindPipeline(rPipeline,
                                       vk::PipelineBindPoint::eRayTracingKHR);
    frame.commandBuffers->bindDescriptorSets(
        rPipelineLayout, descSets, {}, vk::PipelineBindPoint::eRayTracingKHR);

    auto pcData = pushConstantData.get();
    pcData.frameIndex = m_currentFrameNumber;
    pushConstantData.set(pcData);
    frame.commandBuffers->pushConstants<PushConstantRaytrace>(rPipelineLayout,
                                                              pushConstantData);

    frame.commandBuffers->traceRays(sbt, renderTarget.getProperties().size);
    frame.commandBuffers->copyImage(
        renderTarget, *m_swapchain->getImages()[swapchainImgIndex],
        renderTarget.getProperties().size);
    frame.commandBuffers->end();
    vk::PipelineStageFlags pipelineFlags(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_device->graphicsSubmit(frame.presentSemaphore, pipelineFlags,
                             frame.commandBuffers->current[0],
                             frame.renderSemaphore, frame.renderFence);
}

void kirana::viewport::vulkan::Drawer::draw()
{
    if (!m_scene || !m_scene->isInitialized)
        return;

    const vulkan::ShadingPipeline &currShadingPipeline =
        m_scene->getCurrentShadingPipeline();

    if (currShadingPipeline == ShadingPipeline::RAYTRACE)
    {
        if (!m_scene->isRaytracingInitialized ||
            m_currentFrameNumber > constants::VULKAN_RAYTRACING_MAX_SAMPLES)
            return;
    }
    else if (currShadingPipeline == ShadingPipeline::RASTER)
    {
        if (constants::VULKAN_MAX_IDLE_FRAME_COUNT > 0 &&
            m_currentFrameNumber > constants::VULKAN_MAX_IDLE_FRAME_COUNT)
            return;
    }

    const FrameData &frame = getCurrentFrame();

    VK_HANDLE_RESULT(
        m_device->current.waitForFences(frame.renderFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for render fence")

    const vk::ResultValue<uint32_t> imgValue = m_swapchain->acquireNextImage(
        constants::VULKAN_FRAME_SYNC_TIMEOUT, frame.presentSemaphore, nullptr);
    if (imgValue.result == vk::Result::eErrorOutOfDateKHR)
        return;
    else if (imgValue.result != vk::Result::eSuccess &&
             imgValue.result != vk::Result::eSuboptimalKHR)
        VK_HANDLE_RESULT(imgValue.result, "Failed to acquire swapchain image")

    m_device->current.resetFences(frame.renderFence);

    const uint32_t imgIndex = imgValue.value;
    if (currShadingPipeline == ShadingPipeline::RAYTRACE)
    {
        raytrace(frame, imgIndex);
    }
    else
    {
        rasterize(frame, imgIndex);
    }

    vk::Result presentResult = m_device->present(
        frame.renderSemaphore, m_swapchain->current, imgIndex);
    if (presentResult == vk::Result::eErrorOutOfDateKHR)
        return;
    else if (presentResult != vk::Result::eSuccess &&
             presentResult != vk::Result::eSuboptimalKHR)
        VK_HANDLE_RESULT(presentResult, "Failed to present rendered image")

    m_currentFrameNumber++;
}