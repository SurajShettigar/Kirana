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

class Allocator
{
  private:
    bool m_isInitialized = false;
    std::unique_ptr<vma::Allocator> m_current;

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
                  vma::MemoryUsage memoryUsage, AllocatedBuffer *buffer) const;
    bool allocateImage(const vk::ImageCreateInfo &imageCreateInfo,
                        vma::MemoryUsage memoryUsage, vk::MemoryPropertyFlags requiredFlags, AllocateImage *image) const;
    bool mapToMemory(const AllocatedBuffer &buffer, size_t size,
                     const void *data) const;
    void free(const AllocatedBuffer &buffer) const;
    void free(const AllocateImage &image) const;
};
} // namespace kirana::viewport::vulkan
#endif