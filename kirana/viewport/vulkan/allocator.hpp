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
  public:
    /// Used to indicate the type of memory to use for allocation.
    enum class AllocationType
    {
        /// Allocation is done in the GPU memory, and only writeable by GPU
        /// commands. Application cannot write into this allocation.
        GPU_READ_ONLY = 0,
        /// Allocation is done in the GPU memory, but when you want to write
        /// some data, a staging buffer is used internally to transfer the data.
        GPU_WRITEABLE = 1,
        /// Attempts to do allocation in GPU memory which is also host-visible
        /// (writeable by application). If it fails, allocates in GPU memory,
        /// and staging buffer is used to transfer data.
        WRITEABLE = 2,
        /// Allocation is done in memory which supports cached reads by the CPU.
        /// Enables GPU to CPU reads.
        READ_BACK = 3
    };

  private:
    bool m_isInitialized = false;
    std::unique_ptr<vma::Allocator> m_current;

    vk::Fence m_commandFence;
    const CommandPool *m_commandPool;
    const CommandBuffers *m_commandBuffers;

    const Instance *const m_instance;
    const Device *const m_device;

    void displayMemoryInfo();

  public:
    explicit Allocator(const Instance *instance, const Device *device);
    ~Allocator();
    Allocator(const Allocator &buffers) = delete;
    Allocator &operator=(const Allocator &buffers) = delete;

    const bool &isInitialized = m_isInitialized;
    const vma::Allocator &current = *m_current;

    void setCurrentFrameIndex(uint32_t frameIndex);

    bool allocateBuffer(
        AllocatedBuffer *buffer, vk::DeviceSize bufferSize,
        vk::BufferUsageFlags usageFlags,
        AllocationType allocationType = AllocationType::GPU_WRITEABLE,
        const void *data = nullptr, size_t dataOffset = 0,
        size_t dataSize = 0) const;
    bool allocateImage(
        AllocateImage *image, vk::ImageCreateInfo imageCreateInfo,
        vk::ImageLayout layout, vk::ImageSubresourceRange subresourceRange,
        AllocationType allocationType = AllocationType::GPU_WRITEABLE,
        const void *data = nullptr, size_t dataOffset = 0,
        size_t dataSize = 0) const;
    bool copyDataToBuffer(const AllocatedBuffer &buffer, const void *data,
                          size_t dataOffset, size_t dataSize) const;
    [[nodiscard]] bool copyBuffer(const vk::Buffer &stagingBuffer,
                                  const vk::Buffer &destBuffer,
                                  vk::DeviceSize size,
                                  vk::DeviceSize srcOffset = 0,
                                  vk::DeviceSize dstOffset = 0) const;
    void free(const AllocatedBuffer &buffer) const;
    void free(const AllocateImage &image) const;
};
} // namespace kirana::viewport::vulkan
#endif