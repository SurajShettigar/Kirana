#include "vulkan_renderer.hpp"

#include <window.hpp>

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "allocator.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
#include "renderpass.hpp"
#include "descriptor_pool.hpp"
#include "raytrace_data.hpp"
#include "scene_data.hpp"
#include "drawer.hpp"

#include <logger.hpp>
#include <constants.h>

void kirana::viewport::vulkan::VulkanRenderer::init(
    const window::Window *const window, const scene::ViewportScene &scene,
    vulkan::ShadingPipeline pipeline, vulkan::ShadingType type)
{
    m_window = window;
    m_instance = new Instance(m_window->getReqInstanceExtensionsForVulkan());
    if (m_instance && m_instance->isInitialized)
        m_surface = new Surface(m_instance, window);
    if (m_surface && m_surface->isInitialized)
        m_device = new Device(m_instance, m_surface);
    if (m_device && m_device->isInitialized)
    {
        m_allocator = new Allocator(m_instance, m_device);
        m_swapchain = new Swapchain(m_device, m_surface);
        m_swapchainOutOfDateListener =
            m_swapchain->addOnSwapchainOutOfDateListener(
                [&]() { this->rebuildSwapchain(); });
    }
    if (m_swapchain && m_swapchain->isInitialized)
        Texture::createDepthTexture(m_device, m_allocator, m_window->resolution,
                                    m_depthTexture);
    if (m_depthTexture && m_depthTexture->isInitialized)
        m_renderpass = new RenderPass(m_device, m_swapchain, m_depthTexture);

    if (m_renderpass && m_renderpass->isInitialized)
    {
        m_descriptorPool = new DescriptorPool(m_device);
    }
    if (m_descriptorPool && m_descriptorPool->isInitialized)
    {
        m_raytraceData = new RaytraceData(m_device, m_allocator,
                                          m_descriptorPool, m_swapchain);
        m_currentScene =
            new SceneData(m_device, m_allocator, m_descriptorPool, m_renderpass,
                          m_raytraceData, scene, pipeline, type);
    }
    if (m_descriptorPool && m_descriptorPool->isInitialized)
    {
        m_drawer =
            new Drawer(m_device, m_swapchain, m_renderpass, m_currentScene);
    }
    m_isInitialized = m_drawer->isInitialized;
    m_currentFrame = 0;
}

void kirana::viewport::vulkan::VulkanRenderer::update()
{
    if (m_allocator)
        m_allocator->setCurrentFrameIndex(m_currentFrame);
    //    if (m_currentScene)
    //        m_currentScene->updateRaytracedFrameCount();
    m_currentFrame++;
}

void kirana::viewport::vulkan::VulkanRenderer::render()
{
    if (!m_isInitialized)
        return;
    if (!m_isMinimized)
        m_drawer->draw();
    else if (m_window->resolution[0] != 0 && m_window->resolution[1] != 0)
        m_isMinimized = false;
}

void kirana::viewport::vulkan::VulkanRenderer::clean()
{
    if (m_drawer)
    {
        delete m_drawer;
        m_drawer = nullptr;
    }
    if (m_currentScene)
    {
        delete m_currentScene;
        m_currentScene = nullptr;
    }
    if (m_raytraceData)
    {
        delete m_raytraceData;
        m_raytraceData = nullptr;
    }
    if (m_descriptorPool)
    {
        delete m_descriptorPool;
        m_descriptorPool = nullptr;
    }
    if (m_renderpass)
    {
        delete m_renderpass;
        m_renderpass = nullptr;
    }
    if (m_depthTexture)
    {
        delete m_depthTexture;
        m_depthTexture = nullptr;
    }
    if (m_swapchain)
    {
        m_swapchain->removeOnSwapchainOutOfDateListener(
            m_swapchainOutOfDateListener);
        delete m_swapchain;
        m_swapchain = nullptr;
    }
    if (m_allocator)
    {
        delete m_allocator;
        m_allocator = nullptr;
    }
    if (m_device)
    {
        delete m_device;
        m_device = nullptr;
    }
    if (m_surface)
    {
        delete m_surface;
        m_surface = nullptr;
    }
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void kirana::viewport::vulkan::VulkanRenderer::rebuildSwapchain()
{
    m_device->waitUntilIdle();
    if (m_window->resolution[0] == 0 || m_window->resolution[1] == 0)
    {
        m_isMinimized = true;
        return;
    }
    m_isMinimized = false;

    if (m_depthTexture)
    {
        delete m_depthTexture;
        m_depthTexture = nullptr;
    }

    if (m_surface && m_surface->isInitialized)
        m_swapchain->initialize();
    if (m_swapchain && m_swapchain->isInitialized)
    {
        Texture::createDepthTexture(m_device, m_allocator, m_window->resolution,
                                    m_depthTexture);
        m_raytraceData->rebuildRenderTarget();
    }
    if (m_depthTexture && m_depthTexture->isInitialized)
        m_renderpass->initialize(m_depthTexture);

    utils::Logger::get().log(utils::constants::LOG_CHANNEL_VULKAN,
                             utils::LogSeverity::trace, "Swapchain rebuilt");
}

void kirana::viewport::vulkan::VulkanRenderer::setShadingPipeline(
    vulkan::ShadingPipeline pipeline)
{
    m_device->waitUntilIdle();
    m_currentScene->setShadingPipeline(pipeline);
}

void kirana::viewport::vulkan::VulkanRenderer::setShadingType(
    vulkan::ShadingType type)
{
    m_device->waitUntilIdle();
    m_currentScene->setShadingType(type);
}
