#ifndef KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_TYPES_HPP
#define KIRANA_VIEWPORT_VULKAN_FRAME_GRAPH_TYPES_HPP

#include <vector>
#include <functional>

namespace kirana::viewport::vulkan
{
enum class FrameGraphResourceType
{
    UNKNOWN = -1,
    BUFFER = 0,
    TEXTURE = 1,
    ATTACHMENT = 2,
    REFERENCE = 3
};

struct FrameGraphResourceInfo
{
};

struct FrameGraphHandle
{
    uint32_t value;
    explicit FrameGraphHandle(uint32_t handle) : value{handle} {};
    explicit operator uint32_t() const
    {
        return value;
    }
    const uint32_t operator()() const
    {
        return value;
    }
};
struct FrameGraphNodeHandle : FrameGraphHandle
{
    explicit FrameGraphNodeHandle(uint32_t handle)
        : FrameGraphHandle{handle} {};
};
struct FrameGraphResourceHandle : FrameGraphHandle
{
    explicit FrameGraphResourceHandle(uint32_t handle)
        : FrameGraphHandle{handle} {};
};

struct FrameGraphResource
{
    FrameGraphResourceHandle id;
    std::string name;
    uint32_t referenceCount;

    FrameGraphResourceType type;
    FrameGraphResourceInfo info;

    FrameGraphNodeHandle producer;
    FrameGraphResourceHandle outputResource;
};

struct FrameGraphNode {
    FrameGraphNodeHandle id;
    std::string name;
    uint32_t referenceCount;

    std::vector<FrameGraphResourceHandle> inputs;
    std::vector<FrameGraphResourceHandle> outputs;

    std::vector<FrameGraphNodeHandle> connections;
};

} // namespace kirana::viewport::vulkan

// hashing function for handles
template <> struct std::hash<kirana::viewport::vulkan::FrameGraphHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraphHandle const &handle) const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <> struct std::hash<kirana::viewport::vulkan::FrameGraphNodeHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraphNodeHandle const &handle) const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
template <> struct std::hash<kirana::viewport::vulkan::FrameGraphResourceHandle>
{
    std::size_t operator()(
        kirana::viewport::vulkan::FrameGraphResourceHandle const &handle) const noexcept
    {
        return std::hash<std::uint32_t>{}(handle.value);
    }
};
#endif