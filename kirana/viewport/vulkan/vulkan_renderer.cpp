#include "vulkan_renderer.hpp"

#include <window.hpp>

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "allocator.hpp"
#include "swapchain.hpp"
#include "depth_buffer.hpp"
#include "renderpass.hpp"
#include "drawer.hpp"

#include "scene_data.hpp"

void kirana::viewport::vulkan::VulkanRenderer::init(
    const std::vector<const char *> &reqInstanceExtensions,
    const window::Window *const window, const scene::Scene &scene)
{

    m_instance = new Instance(reqInstanceExtensions);
    if (m_instance->isInitialized)
    {
        VkSurfaceKHR surface;
        if (window->getVulkanWindowSurface(
                static_cast<VkInstance>(m_instance->current), nullptr,
                &surface) == VK_SUCCESS)
        {
            m_surface = new Surface(m_instance, vk::SurfaceKHR(surface),
                                    window->getWindowResolution());
        }
    }
    if (m_surface->isInitialized)
        m_device = new Device(m_instance, m_surface);
    if (m_device->isInitialized)
    {
        m_allocator = new Allocator(m_instance, m_device);
        m_swapchain = new Swapchain(m_device, m_surface);
    }
    if (m_swapchain->isInitialized)
        m_depthBuffer = new DepthBuffer(m_device, m_allocator,
                                        window->getWindowResolution());
    if (m_depthBuffer->isInitialized)
        m_renderpass = new RenderPass(m_device, m_swapchain, m_depthBuffer);
    if (m_renderpass->isInitialized)
        m_currentScene = new SceneData(m_allocator, scene);
    if (m_currentScene->isInitialized)
        m_drawer =
            new Drawer(m_device, m_swapchain, m_renderpass, m_currentScene);
}

void kirana::viewport::vulkan::VulkanRenderer::update()
{
}

void kirana::viewport::vulkan::VulkanRenderer::render()
{
    m_drawer->draw();
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

/*
void kirana::viewport::vulkan::VulkanRenderer::loadScene(
    const scene::Scene &scene)
{
    m_currentScene = new SceneData(m_allocator, scene);
    if (m_currentScene->isInitialized)
        m_drawer->loadScene(m_currentScene);
}*/
