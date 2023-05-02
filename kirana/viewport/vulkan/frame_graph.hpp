#ifndef KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_HPP
#define KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_HPP

#include "vulkan_types.hpp"
#include <functional>

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Texture;
class FrameGraph
{
  public:
    struct Handle
    {
        uint32_t value = std::numeric_limits<uint32_t>::max();
        Handle() = default;
        explicit Handle(uint32_t handle) : value{handle} {};
        explicit operator uint32_t() const
        {
            return value;
        }
        uint32_t operator()() const
        {
            return value;
        }
        [[nodiscard]] inline bool isValid() const
        {
            return value < std::numeric_limits<uint32_t>::max();
        }
    };
    struct NodeHandle : Handle
    {
        NodeHandle() = default;
        explicit NodeHandle(uint32_t handle) : Handle{handle} {};
    };
    struct ResourceHandle : Handle
    {
        ResourceHandle() = default;
        explicit ResourceHandle(uint32_t handle) : Handle{handle} {};
    };
    struct FramebufferHandle : Handle
    {
        FramebufferHandle() = default;
        explicit FramebufferHandle(uint32_t handle) : Handle{handle} {};
    };
    struct RenderPassHandle : Handle
    {
        RenderPassHandle() = default;
        explicit RenderPassHandle(uint32_t handle) : Handle{handle} {};
    };
    enum class ResourceType
    {
        UNKNOWN = -1,
        BUFFER = 0,
        TEXTURE = 1,
        ATTACHMENT = 2,
    };
    struct ResourceInfo
    {
        union {
            struct
            {
                const AllocatedBuffer *buffer;
            } buffer;
            struct
            {
                std::array<uint32_t, 3> size;
                vk::AttachmentLoadOp loadOp;
                const Texture *texture;
            } texture;
        };
    };

    struct PassInputResourceData
    {
        /// \brief Name of the input resource.
        std::string name;
        /// \brief Name of the pass which produces the input resource.
        std::string producer;
    };

    struct PassOutputResourceData
    {
        std::string name;
        ResourceType type;
        ResourceInfo info;
    };
    struct PassData
    {
        std::string name;
        std::vector<PassInputResourceData> inputs;
        std::vector<PassOutputResourceData> outputs;
        std::vector<std::string> dependentPasses;
    };

    FrameGraph(const Device *device) : m_device{device} {};
    ~FrameGraph() = default;

    FrameGraph(const FrameGraph &frameGraph) = delete;
    FrameGraph &operator=(const FrameGraph &frameGraph) = delete;

    bool addPasses(const std::vector<PassData> &passes);
    bool compile();
    void render();

  private:
    // Frame-graph structures
    struct Resource
    {
        ResourceHandle id;
        std::string name;
        ResourceType type;
        ResourceInfo info;
    };
    struct NodeInput
    {
        /// \brief The resource handle to be used as input to the node.
        ResourceHandle resource;
        /// \brief The node which is producing the resource.
        NodeHandle producer;
    };
    struct Node
    {
        NodeHandle id;
        std::string name;

        std::vector<NodeInput> inputs;
        std::vector<ResourceHandle> outputs;

        FramebufferHandle framebuffer;
        RenderPassHandle renderPass;
    };

    static const ResourceHandle DEPENDENCY_RESOURCE_HANDLE;
    static const std::string DEPENDENCY_RESOURCE_NAME;

    bool m_isInitialized = false;
    const Device *const m_device;

    std::unordered_map<std::string, uint32_t> m_resourceIndexTable;
    std::unordered_map<std::string, uint32_t> m_nodeIndexTable;
    std::vector<Resource> m_resources;
    std::vector<Node> m_nodes;

    std::vector<const RenderPass *> m_renderPasses;
    std::vector<const Texture *> m_colorAttachments;
    const Texture *m_depthAttachment;

    ResourceHandle addOutputResource(
        const PassOutputResourceData &resourceData);
    NodeHandle addNode(const PassData &passData);
};
} // namespace kirana::viewport::vulkan


// hashing function for handles
template <> struct std::hash<kirana::viewport::vulkan::FrameGraph::Handle>
{
    std::size_t operator()(kirana::viewport::vulkan::FrameGraph::Handle const
                               &handle) const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <> struct std::hash<kirana::viewport::vulkan::FrameGraph::NodeHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraph::NodeHandle const &handle)
        const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <>
struct std::hash<kirana::viewport::vulkan::FrameGraph::ResourceHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraph::ResourceHandle const &handle)
        const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <>
struct std::hash<kirana::viewport::vulkan::FrameGraph::FramebufferHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraph::FramebufferHandle const &handle)
        const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <>
struct std::hash<kirana::viewport::vulkan::FrameGraph::RenderPassHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraph::RenderPassHandle const &handle)
        const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
#endif
