#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <vulkan/vulkan.hpp>
#include <vector>
#include <set>
#include <limits>
#include <memory>
#include <transform.hpp>

namespace vma
{
class Allocation;
}

namespace kirana::viewport::vulkan
{
class DescriptorSet;
class CommandPool;
class CommandBuffers;
class Pipeline;
class PipelineLayout;

enum class ShadingPipeline
{
    RASTER = 0,
    RAYTRACE = 1,
    SHADING_MAX = 2,
};

enum class ShadingType
{
    BASIC = 0,
    UNLIT = 1,
    WIREFRAME = 2,
    BASIC_SHADED_WIREFRAME = 3,
    PBR = 4,
    AO = 5,
    SHADING_TYPE_MAX = 6
};

/**
 * Index of queue families of the selected device.
 */
struct QueueFamilyIndices
{
    uint32_t graphics = std::numeric_limits<uint32_t>::max();
    uint32_t presentation = std::numeric_limits<uint32_t>::max();
    uint32_t transfer = std::numeric_limits<uint32_t>::max();
    uint32_t compute = std::numeric_limits<uint32_t>::max();

    inline bool isGraphicsSupported() const
    {
        return graphics != std::numeric_limits<uint32_t>::max();
    }

    inline bool isPresentationSupported() const
    {
        return presentation != std::numeric_limits<uint32_t>::max();
    }

    inline bool isTransferSupported() const
    {
        return transfer != std::numeric_limits<uint32_t>::max();
    }

    inline bool isComputeSupported() const
    {
        return compute != std::numeric_limits<uint32_t>::max();
    }

    inline bool isGraphicsAndPresentSame() const
    {
        return graphics == presentation;
    }

    inline std::set<uint32_t> getIndices() const
    {
        return std::set<uint32_t>(
            {graphics, presentation, transfer,
             compute}); // Set is used so that each value is unique.
                        // Graphics and presentation queue family can be
                        // same, so this step is necessary.
    }
};

/**
 * Swapchain properties and capabilities of the selected device.
 */
struct SwapchainSupportInfo
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR> presentModes;

    inline bool isSupported() const
    {
        return !surfaceFormats.empty() && !presentModes.empty();
    }
};


/**
 * Holds buffer objects allocated in memory by VMA.
 */
struct AllocatedBuffer
{
    std::unique_ptr<vk::Buffer> buffer = nullptr;
    std::unique_ptr<vma::Allocation> allocation;
    vk::DeviceAddress address;
    vk::DescriptorBufferInfo descInfo;
};

struct BatchBufferData
{
    AllocatedBuffer buffer;
    size_t currentSize = 0;
    size_t currentDataCount = 0;
};

/**
 * Holds an image allocated in memory by VMA.
 */
struct AllocatedImage
{
    std::unique_ptr<vk::Image> image;
    std::unique_ptr<vma::Allocation> allocation;
};

enum class DescriptorLayoutType
{
    GLOBAL = 0,
    MATERIAL = 1,
    OBJECT = 2,
    DESCRIPTOR_LAYOUT_TYPE_MAX = 3
};

enum class DescriptorBindingDataType
{
    CAMERA = 0,
    WORLD = 1,
    RAYTRACE_ACCEL_STRUCT = 2,
    RAYTRACE_RENDER_TARGET = 3,
    MATERIAL_DATA = 4,
    TEXTURE_DATA = 5,
    OBJECT_DATA = 6,
    DESCRIPTOR_BINDING_DATA_TYPE_MAX = 7
};

struct DescriptorBindingInfo
{
    DescriptorLayoutType layoutType;
    uint32_t binding;
    vk::DescriptorType type;
    vk::ShaderStageFlags stages;
    uint32_t descriptorCount = 1;

    bool operator==(const DescriptorBindingInfo &bindingInfo) const
    {
        return layoutType == bindingInfo.layoutType &&
               binding == bindingInfo.binding && type == bindingInfo.type &&
               stages == bindingInfo.stages &&
               descriptorCount == bindingInfo.descriptorCount;
    }

    bool operator!=(const DescriptorBindingInfo &bindingInfo) const
    {
        return !(*this == bindingInfo);
    }
};

struct InstanceData
{
    uint32_t index;
    const math::Transform *transform;
    const bool *viewportVisible;
    const bool *renderVisible;
    const bool *selected;
};

/**
 * Holds the mesh data used by vulkan bindVertexBuffers and draw commands.
 */
struct MeshData
{
    uint32_t index;
    std::string name;
    uint32_t vertexCount;
    uint32_t indexCount;
    int vertexBufferIndex;
    int indexBufferIndex;
    uint32_t firstIndex;
    uint32_t vertexOffset;
    uint32_t materialIndex;
};

struct MeshObjectData
{
    uint32_t index;
    std::string name;
    std::vector<MeshData> meshes;
    std::vector<InstanceData> instances;

    inline uint32_t getGlobalMeshIndex(uint32_t meshIndex) const
    {
        return index + meshes[meshIndex].index;
    }
    inline uint32_t getGlobalInstanceIndex(uint32_t instanceIndex) const
    {
        return index + instances[instanceIndex].index;
    }
};

/**
 * Holds the command pool, command buffers and sync structures per frame.
 */
struct FrameData
{
    vk::Fence renderFence;
    vk::Semaphore renderSemaphore;
    vk::Semaphore presentSemaphore;
    const CommandPool *commandPool = nullptr;
    const CommandBuffers *commandBuffers = nullptr;
};

struct ObjectData
{
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    uint64_t materialDataBufferAddress;
    int materialDataIndex;
    uint32_t firstIndex;
    uint32_t vertexOffset;
};

struct PushConstantRaster
{
    math::Matrix4x4 modelMatrix;
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    uint64_t materialDataBufferAddress;
    int materialDataIndex;
};

struct PushConstantRaytrace
{
    uint32_t frameIndex;
    uint32_t maxBounces;
    uint32_t aaMultiplier;
};

static const vk::ShaderStageFlags PUSH_CONSTANT_RASTER_SHADER_STAGES =
    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
static const vk::ShaderStageFlags PUSH_CONSTANT_RAYTRACE_SHADER_STAGES =
    vk::ShaderStageFlagBits::eRaygenKHR |
    vk::ShaderStageFlagBits::eClosestHitKHR |
    vk::ShaderStageFlagBits::eAnyHitKHR;


/**
 * Raytracing Acceleration Structure
 */
struct ASData
{
    vk::AccelerationStructureKHR as;
    AllocatedBuffer buffer;
};

/**
 * Raytracing Bottom-Level Acceleration Structure
 */
struct BLASData
{
    std::vector<vk::AccelerationStructureGeometryKHR> geometries;
    std::vector<vk::AccelerationStructureBuildRangeInfoKHR> offsets;
    vk::BuildAccelerationStructureFlagsKHR flags;
    vk::AccelerationStructureBuildGeometryInfoKHR buildInfo;
    vk::AccelerationStructureBuildSizesInfoKHR sizeInfo;
    ASData accelStruct;
};

} // namespace kirana::viewport::vulkan

#endif