#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::renderer
{
class Device
{
  private:
    vk::PhysicalDevice m_gpu;
    vk::Device m_current;

    Device(const Device &device) = delete;
    Device &operator=(const Device &device) = delete;

  public:
    Device() = default;
    ~Device() = default;
};
} // namespace kirana::viewport::renderer


#endif