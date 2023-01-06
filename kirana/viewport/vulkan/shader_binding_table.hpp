#ifndef SHADER_BINDING_TABLE_HPP
#define SHADER_BINDING_TABLE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class RaytracePipeline;

class ShaderBindingTable
{
  private:
    bool m_isInitialized = false;
    const Device *const m_device;
    const Allocator *const m_allocator;
    const RaytracePipeline *const m_pipeline;

    AllocatedBuffer m_buffer;
    vk::StridedDeviceAddressRegionKHR m_rayGenRegion {};
    vk::StridedDeviceAddressRegionKHR m_missRegion {};
    vk::StridedDeviceAddressRegionKHR m_hitRegion {};
    vk::StridedDeviceAddressRegionKHR m_callableRegion {};

  public:
    ShaderBindingTable(const Device *device,const Allocator *allocator, const RaytracePipeline *pipeline);
    ~ShaderBindingTable();

    ShaderBindingTable(const ShaderBindingTable &table) = delete;
    ShaderBindingTable &operator=(const ShaderBindingTable &table) = delete;

    const bool &isInitialized = m_isInitialized;
};
} // namespace kirana::viewport::vulkan
#endif