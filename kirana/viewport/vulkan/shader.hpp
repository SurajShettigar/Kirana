#ifndef SHADER_HPP
#define SHADER_HPP

#include <unordered_map>
#include "vulkan_types.hpp"

namespace kirana::scene
{
class ShaderData;
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
    enum class DescriptorFrequency
    {
        GLOBAL,
        MATERIAL,
        OBJECT
    };

    bool m_isInitialized = false;
    std::string m_name = "";
    std::unordered_map<vk::ShaderStageFlagBits, vk::ShaderModule> m_stages;
    std::vector<const DescriptorSetLayout *> m_descLayouts;
    std::vector<const PushConstantBase *> m_pushConstants;
    PipelineLayout *m_pipelineLayout;

    const Device *const m_device;

    static bool readShaderFile(const char *path, std::vector<uint32_t> *buffer);

    static std::vector<DescriptorData> getDescriptors(
        DescriptorFrequency frequency, scene::ShadingPipeline pipeline);
    bool createPipelineLayout(scene::ShadingPipeline pipeline);

  public:
    explicit Shader(const Device *device, const scene::ShaderData &shaderData);
    ~Shader();
    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;

    inline bool hasShaderStage(vk::ShaderStageFlagBits stage) const
    {
        return m_stages.find(stage) != m_stages.end();
    }

    [[nodiscard]] inline const std::unordered_map<vk::ShaderStageFlagBits,
                                                  vk::ShaderModule>
        &getAllModules() const
    {
        return m_stages;
    }

    [[nodiscard]] inline const vk::ShaderModule &getShaderModule(
        vk::ShaderStageFlagBits stage) const
    {
        return m_stages.at(stage);
    }
};
} // namespace kirana::viewport::vulkan


#endif // SHADER_HPP
