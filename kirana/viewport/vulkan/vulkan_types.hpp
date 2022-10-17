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

// TODO: Remove it once descriptor set is implemented.
struct MeshPushConstants
{
    math::Matrix4x4 renderMatrix;
};

/**
 * Holds the mesh material data such as shader, pipeline layout and pipeline
 * for each mesh.
 */
struct MaterialData
{
    std::string name;
    std::string shaderName;
    std::unique_ptr<PipelineLayout> layout;
    std::unique_ptr<Pipeline> pipeline;
};

/**
 * Holds the mesh data used by vulkan bindVertexBuffers and draw commands.
 */
struct MeshData
{
    AllocatedBuffer vertexBuffer;
    vk::DeviceSize vertexOffset = 0;
    size_t vertexCount;
    std::vector<math::Transform *> instanceTransforms;
    MaterialData *material;
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

} // namespace kirana::viewport::vulkan

#endif