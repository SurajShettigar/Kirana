#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory>
#include <vulkan_types.hpp>

namespace vma
{
class Allocator;
enum class MemoryUsage;
} // namespace vma

namespace kirana::scene
{
struct Vertex;
}

namespace kirana::viewport::vulkan
{
class Instance;
class Device;
class CommandPool;
class CommandBuffers;

class Allocator
{
  private:
    bool m_isInitialized = false;
    std::unique_ptr<vma::Allocator> m_current;

    vk::Fence m_commandFence;
    const CommandPool *m_commandPool;
    const CommandBuffers *m_commandBuffers;

    const Instance *const m_instance;
    const Device *const m_device;

  public:
    explicit Allocator(const Instance *instance, const Device *device);
    ~Allocator();
    Allocator(const Allocator &buffers) = delete;
    Allocator &operator=(const Allocator &buffers) = delete;

    const bool &isInitialized = m_isInitialized;
    const vma::Allocator &current = *m_current;

    bool allocateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
                        vma::MemoryUsage memoryUsage, AllocatedBuffer *buffer,
                        bool mapMemoryPointer = true) const;
    bool allocateBufferToGPU(vk::DeviceSize size,
                             vk::BufferUsageFlags usageFlags,
                             AllocatedBuffer *buffer, const void *data) const;
    bool allocateImage(const vk::ImageCreateInfo &imageCreateInfo,
                       vk::ImageLayout layout,
                       vk::ImageSubresourceRange subresourceRange,
                       vma::MemoryUsage memoryUsage,
                       vk::MemoryPropertyFlags requiredFlags,
                       AllocateImage *image) const;
    bool copyDataToMemory(AllocatedBuffer &buffer, size_t size, uint32_t offset,
                          const void *data) const;
    [[nodiscard]] bool copyBuffer(const AllocatedBuffer &stagingBuffer,
                                  const AllocatedBuffer &destBuffer,
                                  vk::DeviceSize size,
                                  vk::DeviceSize srcOffset = 0,
                                  vk::DeviceSize dstOffset = 0) const;
    void free(const AllocatedBuffer &buffer) const;
    void free(const AllocateImage &image) const;
};
} // namespace kirana::viewport::vulkan
#endif