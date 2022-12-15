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

    std::vector<BLASData> m_BLASData;
    std::vector<vk::AccelerationStructureInstanceKHR> m_TLASInstanceData;
    ASData m_TLASData;


    const Device *const m_device;
    const Allocator *const m_allocator;
    const CommandPool *m_commandPool;

    static vk::TransformMatrixKHR getVulkanTransformMatrix(
        const math::Matrix4x4 &matrix);

    [[nodiscard]] bool createAccelerationStructure(
        const vk::AccelerationStructureBuildSizesInfoKHR &sizeInfo,
        vk::AccelerationStructureTypeKHR type,
        ASData *accelerationStructure) const;
    [[nodiscard]] vk::DeviceAddress getBLASAddress(uint32_t meshIndex) const;

    void createBLAS(const std::unordered_map<std::string, MeshData> &meshes);

    bool buildBLAS(
        vk::BuildAccelerationStructureFlagsKHR flags =
            vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);

    void createTLAS(const std::unordered_map<std::string, MeshData> &meshes);

    bool buildTLAS(
        vk::BuildAccelerationStructureFlagsKHR flags =
            vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace,
        bool update = false);

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