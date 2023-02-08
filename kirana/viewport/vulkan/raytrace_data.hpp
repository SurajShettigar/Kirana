#ifndef RAYTRACE_DATA_HPP
#define RAYTRACE_DATA_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class DescriptorPool;
class Swapchain;
class RaytracePipeline;
class ShaderBindingTable;
class AccelerationStructure;
class PipelineLayout;
class DescriptorSet;
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
    const DescriptorPool *const m_descriptorPool;
    const Swapchain *m_swapchain;
    const RaytracePipeline *m_currentPipeline = nullptr;
    const ShaderBindingTable *m_currentSBT = nullptr;
    AccelerationStructure *m_accelStruct = nullptr;
    Texture *m_renderTarget = nullptr;
    // TODO: Switch to per-shader pipeline layout using shader reflection.
    const PipelineLayout *m_raytracePipelineLayout = nullptr;
    // TODO: Switch to per-shader descriptor set using shader reflection.
    std::vector<DescriptorSet> m_descSets;

    void bindDescriptorSets(const SceneData &sceneData);
    bool createRenderTarget();
    bool createAccelerationStructure(const SceneData &sceneData);

  public:
    RaytraceData(const Device *device, const Allocator *allocator,
                 const DescriptorPool *descriptorPool,
                 const Swapchain *swapchain);
    ~RaytraceData();

    RaytraceData(const RaytraceData &raytraceData) = delete;
    RaytraceData &operator=(const RaytraceData &raytraceData) = delete;


    bool initialize(const SceneData &sceneData);
    void updateDescriptors(int setIndex = -1);

    void setPipeline(const RaytracePipeline *pipeline,
                     const ShaderBindingTable *sbt);

    void rebuildRenderTarget();

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

    [[nodiscard]] inline const Texture &getRenderTarget() const
    {
        assert(m_renderTarget != nullptr);
        return *m_renderTarget;
    }

    [[nodiscard]] inline const PipelineLayout &getRaytracePipelineLayout() const
    {
        assert(m_raytracePipelineLayout != nullptr);
        return *m_raytracePipelineLayout;
    }

    [[nodiscard]] inline const std::vector<DescriptorSet> &getDescriptorSets()
        const
    {
        return m_descSets;
    }

    const bool &isInitialized = m_isInitialized;
};
} // namespace kirana::viewport::vulkan

#endif