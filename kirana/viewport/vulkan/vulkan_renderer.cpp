#include "vulkan_renderer.hpp"

#include <window.hpp>

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "allocator.hpp"
#include "swapchain.hpp"
#include "depth_buffer.hpp"
#include "renderpass.hpp"
#include "descriptor_pool.hpp"
#include "scene_data.hpp"
#include "drawer.hpp"

#include <logger.hpp>
#include <constants.h>

void kirana::viewport::vulkan::VulkanRenderer::init(
    const window::Window *const window, const scene::ViewportScene &scene,
    uint16_t shadingIndex)
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
    }
    if (m_swapchain && m_swapchain->isInitialized)
        m_depthBuffer =
            new DepthBuffer(m_device, m_allocator, m_window->resolution);
    if (m_depthBuffer && m_depthBuffer->isInitialized)
        m_renderpass = new RenderPass(m_device, m_swapchain, m_depthBuffer);

    if (m_renderpass && m_renderpass->isInitialized)
    {
        m_descriptorPool = new DescriptorPool(m_device);
    }
    if (m_descriptorPool && m_descriptorPool->isInitialized)
    {
        m_currentScene =
            new SceneData(m_device, m_allocator, m_renderpass, scene, shadingIndex);
    }
    if (m_descriptorPool && m_descriptorPool->isInitialized)
    {
        m_drawer = new Drawer(m_device, m_allocator, m_descriptorPool, m_swapchain, m_renderpass,
                              m_currentScene);
        m_swapchainOutOfDateListener =
            m_drawer->addOnSwapchainOutOfDateListener(
                [&]() { this->rebuildSwapchain(); });
    }
}

void kirana::viewport::vulkan::VulkanRenderer::update()
{

}

void kirana::viewport::vulkan::VulkanRenderer::render()
{
    if (!m_isMinimized)
        m_drawer->draw();
    else if (m_window->resolution[0] != 0 && m_window->resolution[1] != 0)
        m_isMinimized = false;
}

void kirana::viewport::vulkan::VulkanRenderer::clean()
{
    if (m_drawer)
    {
        m_drawer->removeOnSwapchainOutOfDateListener(
            m_swapchainOutOfDateListener);
        delete m_drawer;
        m_drawer = nullptr;
    }
    if (m_currentScene)
    {
        delete m_currentScene;
        m_currentScene = nullptr;
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
    if (m_depthBuffer)
    {
        delete m_depthBuffer;
        m_depthBuffer = nullptr;
    }
    if (m_swapchain)
    {
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
    if (m_window->resolution[0] == 0 || m_window->resolution[1] == 0)
    {
        m_isMinimized = true;
        return;
    }
    m_isMinimized = false;

    if (m_renderpass)
    {
        delete m_renderpass;
        m_renderpass = nullptr;
    }
    if (m_depthBuffer)
    {
        delete m_depthBuffer;
        m_depthBuffer = nullptr;
    }
    if (m_swapchain)
    {
        delete m_swapchain;
        m_swapchain = nullptr;
    }
    m_device->reinitializeSwapchainInfo();
    if (m_surface && m_surface->isInitialized)
        m_swapchain = new Swapchain(m_device, m_surface);
    if (m_swapchain && m_swapchain->isInitialized)
        m_depthBuffer =
            new DepthBuffer(m_device, m_allocator, m_window->resolution);
    if (m_depthBuffer && m_depthBuffer->isInitialized)
        m_renderpass = new RenderPass(m_device, m_swapchain, m_depthBuffer);
    if (m_renderpass && m_renderpass->isInitialized)
        m_currentScene->rebuildPipeline(m_renderpass);
    m_drawer->reinitialize(m_swapchain, m_renderpass);
    utils::Logger::get().log(utils::constants::LOG_CHANNEL_VULKAN,
                             utils::LogSeverity::trace, "Swapchain rebuilt");
}

void kirana::viewport::vulkan::VulkanRenderer::setShading(uint16_t shadingIndex)
{
    m_device->waitUntilIdle();
    m_currentScene->setShading(shadingIndex);
    /*switch (shadingIndex)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }*/
}

/*
void kirana::viewport::vulkan::VulkanRenderer::loadScene(
    const scene::Scene &scene)
{
    m_currentScene = new SceneData(m_allocator, scene);
    if (m_currentScene->isInitialized)
        m_drawer->loadScene(m_currentScene);
}*/
