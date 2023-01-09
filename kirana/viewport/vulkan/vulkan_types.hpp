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

/**
 * Index of queue families of the selected device.
 */
struct QueueFamilyIndices
{
    uint32_t graphics = std::numeric_limits<uint32_t>::max();
    uint32_t presentation = std::numeric_limits<uint32_t>::max();

    inline bool isGraphicsSupported() const
    {
        return graphics != std::numeric_limits<uint32_t>::max();
    }

    inline bool isPresentationSupported() const
    {
        return presentation != std::numeric_limits<uint32_t>::max();
    }

    inline bool isGraphicsAndPresentSame() const
    {
        return graphics == presentation;
    }

    inline std::set<uint32_t> getIndices() const
    {
        return std::set<uint32_t>(
            {graphics,
             presentation}); // Set is used so that each value is unique.
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
    std::unique_ptr<vk::Buffer> buffer;
    std::unique_ptr<vma::Allocation> allocation;
    void *memoryPointer = nullptr;
    vk::DescriptorBufferInfo descInfo;
};

/**
 * Holds an image allocated in memory by VMA.
 */
struct AllocateImage
{
    std::unique_ptr<vk::Image> image;
    std::unique_ptr<vma::Allocation> allocation;
};

/**
 * Holds list of bindings and attributes of vertices to define vertices to the
 * vulkan pipeline.
 */
struct VertexInputDescription
{
    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributes;
};

enum class ShaderStage
{
    COMPUTE = 0,
    VERTEX = 1,
    FRAGMENT = 2,
    RAYTRACE_RAY_GEN = 3,
    RAYTRACE_MISS = 4,
    RAYTRACE_CLOSEST_HIT = 5,
    SHADER_STAGE_MAX = 6
};

// TODO: Remove it once descriptor set is implemented.
struct MeshPushConstants
{
    math::Matrix4x4 renderMatrix;
};

/**
 * Holds the Camera view and projection matrix.
 */
struct CameraData
{
    math::Matrix4x4 viewMatrix;
    math::Matrix4x4 projectionMatrix;
    math::Matrix4x4 viewProjectionMatrix;
    math::Vector3 position;
    alignas(16) math::Vector3 direction;
    float nearPlane;
    alignas(4) float farPlane;
};

struct ObjectData
{
    math::Matrix4x4 modelMatrix;
};

struct InstanceData
{
    uint32_t index;
    const math::Transform *transform;
    const bool *selected;
    const bool *renderVisible;
};

/**
 * Holds the mesh data used by vulkan bindVertexBuffers and draw commands.
 */
struct MeshData
{
    uint32_t index;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    std::vector<InstanceData> instances;
    const Pipeline *material;
    bool render = false;

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
    const DescriptorSet *globalDescriptorSet = nullptr;
    const DescriptorSet *objectDescriptorSet = nullptr;
    const DescriptorSet *raytraceDescriptorSet = nullptr;
    vk::Fence renderFence;
    vk::Semaphore renderSemaphore;
    vk::Semaphore presentSemaphore;
    const CommandPool *commandPool = nullptr;
    const CommandBuffers *commandBuffers = nullptr;
};

struct RaytracedGlobalData
{
    uint64_t vertexBufferAddress;
    alignas(8) uint64_t indexBufferAddress;
};

struct RaytracedObjectData
{
    math::Matrix4x4 modelMatrix;
    uint32_t firstIndex;
    alignas(4) uint32_t vertexOffset;
    alignas(4) uint32_t padding1;
    alignas(4) uint32_t padding2;
};

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