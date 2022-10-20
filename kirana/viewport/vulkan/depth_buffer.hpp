#ifndef DEPTH_BUFFER_HPP
#define DEPTH_BUFFER_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;

class DepthBuffer
{
  private:
    bool m_isInitialized = false;
    vk::Format m_format;
    AllocateImage m_image;
    vk::ImageView m_imageView;

    const Device *const m_device;
    const Allocator *const m_allocator;

  public:
    explicit DepthBuffer(const Device *device, const Allocator *allocator,
                         const std::array<uint32_t, 2> &windowResolution);
    ~DepthBuffer();
    DepthBuffer(const DepthBuffer &buffer) = delete;
    DepthBuffer &operator=(const DepthBuffer &buffer) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Format &format = m_format;
    const vk::ImageView &imageView = m_imageView;
};
} // namespace kirana::viewport::vulkan
#endif