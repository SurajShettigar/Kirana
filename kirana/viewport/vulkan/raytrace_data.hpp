#ifndef RAYTRACE_DATA_HPP
#define RAYTRACE_DATA_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class Swapchain;
class RaytracePipeline;
class ShaderBindingTable;
class AccelerationStructure;
class Texture;
class SceneData;

class RaytraceData
{
  public:
    static const vk::BufferUsageFlagBits VERTEX_INDEX_BUFFER_USAGE_FLAGS;
  private:
    bool m_isInitialized = false;
    const Device *const m_device;
    const Allocator *const m_allocator;
    const Swapchain *m_swapchain;
    const RaytracePipeline *m_currentPipeline = nullptr;
    const ShaderBindingTable *m_currentSBT = nullptr;
    AccelerationStructure *m_accelStruct = nullptr;
    Texture *m_renderTarget = nullptr;

    bool createRenderTarget();
  public:

    RaytraceData(const Device *device, const Allocator *allocator,
                 const Swapchain *swapchain);
    ~RaytraceData();

    RaytraceData(const RaytraceData &raytraceData) = delete;
    RaytraceData &operator=(const RaytraceData &raytraceData) = delete;

    bool buildAccelerationStructure(const SceneData &sceneData);
    void setPipeline(const RaytracePipeline *pipeline,
                     const ShaderBindingTable *sbt);

    void rebuildRenderTarget(const Swapchain *newSwapchain);

    [[nodiscard]] inline const RaytracePipeline &getCurrentPipeline() const
    {
        assert(m_currentPipeline != nullptr);
        return *m_currentPipeline;
    }

    [[nodiscard]] inline const ShaderBindingTable &getCurrentSBT() const
    {
        assert(m_currentSBT != nullptr);
        return *m_currentSBT;
    }

    [[nodiscard]] inline const AccelerationStructure &getAccelerationStructure()
        const
    {
        assert(m_accelStruct != nullptr);
        return *m_accelStruct;
    }

    const bool &isInitialized = m_isInitialized;
};
} // namespace kirana::viewport::vulkan

#endif