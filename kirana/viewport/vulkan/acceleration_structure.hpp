#ifndef ACCELERATION_STRUCTURE_HPP
#define ACCELERATION_STRUCTURE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class AccelerationStructure
{
  private:
    std::vector<BLASData> m_BLASInputData;

    const Device *const m_device;

    void buildBLAS();
  public:
    AccelerationStructure(
        const Device *device,
        const std::unordered_map<std::string, MeshData> &meshes);
    ~AccelerationStructure();
};
} // namespace kirana::viewport::vulkan
#endif