#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class CommandPool;
class CommandBuffers
{
  private:
    bool m_isInitialized = false;
    std::vector<vk::CommandBuffer> m_current;

    const Device *const m_device;
    const CommandPool *const m_commandPool;

  public:
    explicit CommandBuffers(
        const Device *device, const CommandPool *commandPool, size_t count = 1,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
    ~CommandBuffers() = default;
    CommandBuffers(const CommandBuffers &buffers) = delete;
    CommandBuffers &operator=(const CommandBuffers &buffers) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::vector<vk::CommandBuffer> &current = m_current;
};
} // namespace kirana::viewport::vulkan


#endif
