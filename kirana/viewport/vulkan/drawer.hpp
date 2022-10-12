#ifndef DRAWER_HPP
#define DRAWER_HPP

#include <vulkan/vulkan.hpp>
#include <perspective_camera.hpp>
#include <orthographic_camera.hpp>

namespace kirana::viewport::vulkan
{
class CommandPool;
class CommandBuffers;
class Device;
class Swapchain;
class RenderPass;
class PipelineLayout;
class Pipeline;
class Shader;
class SceneData;

class Drawer
{
  private:
    bool m_isInitialized = false;
    uint64_t m_currentFrameNumber = 0;
    vk::Fence m_renderFence;
    vk::Semaphore m_presentSemaphore;
    vk::Semaphore m_renderSemaphore;

    CommandPool *m_commandPool;
    CommandBuffers *m_mainCommandBuffers;

    Shader *m_shader;
    PipelineLayout *m_trianglePipelineLayout;
    Pipeline *m_trianglePipeline;

    const Device *const m_device;
    const Swapchain *const m_swapchain;
    const RenderPass *const m_renderPass;

    const SceneData *const m_scene;

    camera::PerspectiveCamera m_camera;
//    camera::OrthographicCamera m_camera;
//    math::Transform m_model;
  public:
    explicit Drawer(const Device *device, const Swapchain *swapchain,
                    const RenderPass *renderPass, const SceneData *scene);
    ~Drawer();
    Drawer(const Drawer &drawer) = delete;
    Drawer &operator=(const Drawer &drawer) = delete;

    const bool &isInitialized = m_isInitialized;

    /// The Vulkan draw calls and synchronization between them are executed
    /// here.
    void draw();

    //    void loadScene(SceneData *scene);
};
} // namespace kirana::viewport::vulkan


#endif // DRAWER_HPP
