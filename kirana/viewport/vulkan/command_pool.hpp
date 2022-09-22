#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;

class CommandPool
{
  private:
    bool m_isInitialized = false;
    vk::CommandPool m_current;

    const Device *const m_device;

  public:
    explicit CommandPool(const Device *device, uint32_t queueFamilyIndex = 0,
                vk::CommandPoolCreateFlagBits flags =
                    vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    ~CommandPool();
    CommandPool(const CommandPool &pool) = delete;
    CommandPool &operator=(const CommandPool &pool) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::CommandPool &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif