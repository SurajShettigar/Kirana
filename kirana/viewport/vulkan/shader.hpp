#ifndef SHADER_HPP
#define SHADER_HPP

#include <unordered_map>
#include "vulkan_types.hpp"

namespace kirana::scene
{
struct ShaderData;
enum class ShadingPipeline;
} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Device;
class DescriptorSetLayout;
class PushConstantBase;
class PipelineLayout;
class Shader
{
  private:
    bool m_isInitialized = false;
    const Device *const m_device;
    std::string m_name = "";
    vulkan::ShadingPipeline m_shadingPipeline = vulkan::ShadingPipeline::RASTER;
    std::unordered_map<vk::ShaderStageFlagBits, std::vector<vk::ShaderModule>>
        m_stages;
    const PipelineLayout *m_pipelineLayout;

    static bool readShaderFile(const char *path, std::vector<uint32_t> *buffer);
  public:
    explicit Shader(const Device *device, const scene::ShaderData &shaderData);
    ~Shader();
    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
    const vulkan::ShadingPipeline &shadingPipeline = m_shadingPipeline;

    inline bool hasShaderStage(vk::ShaderStageFlagBits stage) const
    {
        return m_stages.find(stage) != m_stages.end();
    }

    [[nodiscard]] inline const std::unordered_map<vk::ShaderStageFlagBits,
                                                  std::vector<vk::ShaderModule>>
        &getAllModules() const
    {
        return m_stages;
    }

    [[nodiscard]] inline const std::vector<vk::ShaderModule> &getShaderModule(
        vk::ShaderStageFlagBits stage) const
    {
        return m_stages.at(stage);
    }

    [[nodiscard]] inline const PipelineLayout &getPipelineLayout() const
    {
        return *m_pipelineLayout;
    }
};
} // namespace kirana::viewport::vulkan


#endif // SHADER_HPP
