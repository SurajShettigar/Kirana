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
  public:
    enum class GroupType
    {
        RAY_GEN = 0,
        MISS = 1,
        HIT = 2,
        CALLABLE = 3,
        GROUP_TYPE_MAX = 4
    };

  private:
    struct Group
    {
        GroupType type = GroupType::RAY_GEN;
        std::vector<uint32_t>
            recordIndices; // Used to get handles from pipeline. Index value is
                           // determined by the stage index when the pipeline is
                           // created.
        uint32_t recordCount = 0;
        uint32_t stride = 0;
        uint32_t size = 0;
        AllocatedBuffer buffer;
        vk::DeviceAddress bufferAddress = 0;

        explicit operator std::string() const
        {
            switch (type)
            {
            case GroupType::RAY_GEN:
                return "RAY_GEN";
            case GroupType::MISS:
                return "MISS";
            case GroupType::HIT:
                return "HIT";
            case GroupType::CALLABLE:
                return "CALLABLE";
            default:
                return "UNKNOWN";
            }
        }

        [[nodiscard]] inline vk::StridedDeviceAddressRegionKHR getRegion(
            uint32_t indexOffset = 0) const
        {
            if (bufferAddress == 0)
                return vk::StridedDeviceAddressRegionKHR{};
            else
                return vk::StridedDeviceAddressRegionKHR{
                    bufferAddress +
                        static_cast<vk::DeviceAddress>(indexOffset * stride),
                    stride, type == GroupType::RAY_GEN ? stride : size};
        }
    };

    bool m_isInitialized = false;
    const Device *const m_device;
    const Allocator *const m_allocator;
    const RaytracePipeline *const m_pipeline;
    std::string m_name;

    uint32_t m_handleSize = 0;
    uint32_t m_handleSizeAligned = 0;
    std::array<Group, static_cast<int>(GroupType::GROUP_TYPE_MAX)> m_groups;
    uint32_t m_totalGroupCount = 0;

    void initializeShaderRecords();
    [[nodiscard]] std::array<std::vector<uint8_t>,
                             static_cast<int>(GroupType::GROUP_TYPE_MAX)>
    getBufferData() const;

  public:
    ShaderBindingTable(const Device *device, const Allocator *allocator,
                       const RaytracePipeline *pipeline);
    ~ShaderBindingTable();

    ShaderBindingTable(const ShaderBindingTable &table) = delete;
    ShaderBindingTable &operator=(const ShaderBindingTable &table) = delete;

    [[nodiscard]] inline vk::StridedDeviceAddressRegionKHR getGroupRegion(
        GroupType groupType, uint32_t indexOffset = 0) const
    {
        return m_groups.at(static_cast<int>(groupType)).getRegion(indexOffset);
    }

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
};
} // namespace kirana::viewport::vulkan
#endif