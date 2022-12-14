#ifndef ACCELERATION_STRUCTURE_HPP
#define ACCELERATION_STRUCTURE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class CommandPool;
class CommandBuffers;
class AccelerationStructure
{
  private:
    bool m_isInitialized = false;
    std::vector<BLASData> m_BLASInputData;
    std::vector<BuildASData> m_BLASBuildData;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const CommandPool *m_commandPool;

    bool buildBLAS(
        vk::BuildAccelerationStructureFlagsKHR flags =
            vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);

  public:
    AccelerationStructure(
        const Device *device, const Allocator *allocator,
        const std::unordered_map<std::string, MeshData> &meshes);
    ~AccelerationStructure();
    AccelerationStructure(const AccelerationStructure &as) = delete;
    AccelerationStructure &operator=(const AccelerationStructure &as) = delete;

    const bool &isInitialized = m_isInitialized;
};
} // namespace kirana::viewport::vulkan
#endif