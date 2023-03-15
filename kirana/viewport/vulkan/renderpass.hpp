#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class Swapchain;
class Texture;

class RenderPass
{
  public:
    enum class RenderPassType
    {
        FIRST = 0,
        OFFSCREEN = 1,
        LAST = 2
    };
    struct Properties
    {
        bool hasDepth = true;
        bool clearColor = true;
        bool clearDepth = true;
        RenderPassType type = RenderPassType::FIRST;
        vk::Format colorFormat = vk::Format::eR8G8B8A8Srgb;
        vk::Format depthFormat = vk::Format::eD32SfloatS8Uint;
    };
    explicit RenderPass(const Device *device, Properties properties);
    ~RenderPass();
    RenderPass(const RenderPass &renderpass) = delete;
    RenderPass &operator=(const RenderPass &renderpass) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::RenderPass &current = m_current;

    [[nodiscard]] inline const Properties &getProperties() const
    {
        return m_properties;
    }

  private:
    bool m_isInitialized = false;
    const Device *const m_device;
    vk::RenderPass m_current = nullptr;
    Properties m_properties;
};
} // namespace kirana::viewport::vulkan

#endif // RENDERPASS_HPP
