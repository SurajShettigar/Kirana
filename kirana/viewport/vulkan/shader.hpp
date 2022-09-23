#ifndef SHADER_HPP
#define SHADER_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Shader
{
  private:
    bool m_isInitialized = false;
    const char *m_name;
    vk::ShaderModule m_vertex;
    vk::ShaderModule m_fragment;

    const Device *const m_device;

    static bool readShaderFile(const char *path,
                               std::vector<uint32_t> *buffer);

  public:
    explicit Shader(const Device *device, const char *name);
    ~Shader();
    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::ShaderModule &vertex = m_vertex;
    const vk::ShaderModule &fragment = m_fragment;
};
} // namespace kirana::viewport::vulkan


#endif // SHADER_HPP
