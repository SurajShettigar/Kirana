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
    vk::DescriptorBufferInfo descInfo;
};

/**
 * Holds an image allocated in memory by VMA.
 */
struct AllocateImage
{
    std::unique_ptr<vk::Image> image;
    std::unique_ptr<vma::Allocation> allocation;
    vk::DescriptorImageInfo descInfo;
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

struct PipelineProperties
{
    vk::PrimitiveTopology primitiveType = vk::PrimitiveTopology::eTriangleList;
    vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags cullMode = vk::CullModeFlagBits::eBack;
    float lineWidth = 1.0f;
    vk::SampleCountFlagBits msaaLevel = vk::SampleCountFlagBits::e1;
    bool alphaBlending = false;
    bool enableDepth = true;
    bool writeDepth = true;
    vk::CompareOp depthCompareOp = vk::CompareOp::eLessOrEqual;
    bool stencilTest = false;
    vk::CompareOp stencilCompareOp = vk::CompareOp::eAlways;
    vk::StencilOp stencilFailOp = vk::StencilOp::eReplace;
    vk::StencilOp stencilDepthFailOp = vk::StencilOp::eReplace;
    vk::StencilOp stencilPassOp = vk::StencilOp::eReplace;
    uint32_t stencilReference = 1;
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

/**
 * Holds the mesh material data such as shader, pipeline layout and pipeline
 * for each mesh.
 */
struct MaterialData
{
    std::string name;
    std::string shaderName;
    PipelineProperties properties;
    std::unique_ptr<PipelineLayout> layout;
    std::unique_ptr<Pipeline> pipeline;
};

struct InstanceData
{
    const math::Transform *transform;
    const bool *selected;
};

/**
 * Holds the mesh data used by vulkan bindVertexBuffers and draw commands.
 */
struct MeshData
{
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;
    size_t vertexCount;
    size_t indexCount;
    std::vector<InstanceData> instances;
    MaterialData *material;
};

/**
 * Holds the command pool, command buffers and sync structures per frame.
 */
struct FrameData
{
    const DescriptorSet *globalDescriptorSet = nullptr;
    vk::Fence renderFence;
    vk::Semaphore renderSemaphore;
    vk::Semaphore presentSemaphore;
    const CommandPool *commandPool = nullptr;
    const CommandBuffers *commandBuffers = nullptr;
};

} // namespace kirana::viewport::vulkan

#endif