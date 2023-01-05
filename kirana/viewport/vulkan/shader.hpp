#ifndef SHADER_HPP
#define SHADER_HPP

#include <unordered_map>
#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Shader
{
  private:
    bool m_isInitialized = false;
    std::string m_name = "MatCap";
    std::unordered_map<ShaderStage, vk::ShaderModule> m_stages;

    const Device *const m_device;

    static std::string getPathForShaderStage(const std::string &name,
                                             ShaderStage stage);
    static bool readShaderFile(const char *path, std::vector<uint32_t> *buffer);

  public:
    explicit Shader(const Device *device, const std::string &name = "");
    ~Shader();
    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;

    inline bool hasShaderStage(ShaderStage stage) const
    {
        return m_stages.find(stage) != m_stages.end();
    }

    [[nodiscard]] inline const std::unordered_map<ShaderStage, vk::ShaderModule>
        &getAllModules() const
    {
        return m_stages;
    }

    [[nodiscard]] inline const vk::ShaderModule &getShaderModule(
        ShaderStage stage) const
    {
        return m_stages.at(stage);
    }
};
} // namespace kirana::viewport::vulkan


#endif // SHADER_HPP
