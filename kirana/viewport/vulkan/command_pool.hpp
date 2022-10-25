#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class CommandBuffers;
class CommandPool
{
  private:
    bool m_isInitialized = false;
    vk::CommandPool m_current;

    const Device *const m_device;

  public:
    explicit CommandPool(
        const Device *device, uint32_t queueFamilyIndex = 0,
        vk::CommandPoolCreateFlagBits flags =
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    ~CommandPool();
    CommandPool(const CommandPool &pool) = delete;
    CommandPool &operator=(const CommandPool &pool) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::CommandPool &current = m_current;

    bool allocateCommandBuffers(
        const CommandBuffers *&commandBuffers, size_t count = 1,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) const;
    void reset(vk::CommandPoolResetFlags resetFlags =
                   vk::CommandPoolResetFlagBits::eReleaseResources) const;
};
} // namespace kirana::viewport::vulkan
#endif