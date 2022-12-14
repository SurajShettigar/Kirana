#ifndef SHADER_HPP
#define SHADER_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Device;
class Shader
{
  private:
    bool m_isInitialized = false;
    std::string m_name = "MatCap";
    vk::ShaderModule m_compute = nullptr;
    vk::ShaderModule m_vertex = nullptr;
    vk::ShaderModule m_fragment = nullptr;

    const Device *const m_device;

    static bool readShaderFile(const char *path, std::vector<uint32_t> *buffer);

  public:
    explicit Shader(const Device *device, const std::string &name = "");
    ~Shader();
    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
    const vk::ShaderModule &compute = m_compute;
    const vk::ShaderModule &vertex = m_vertex;
    const vk::ShaderModule &fragment = m_fragment;
};
} // namespace kirana::viewport::vulkan


#endif // SHADER_HPP
