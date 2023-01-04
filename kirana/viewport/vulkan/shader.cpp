#include "shader.hpp"
#include "device.hpp"
#include "vulkan_utils.hpp"

#include <fstream>
#include <string>
#include <file_system.hpp>


std::string kirana::viewport::vulkan::Shader::getPathForShaderStage(
    const std::string &name, ShaderStage stage)
{
    const char *extension = "";
    switch (stage)
    {
    case ShaderStage::COMPUTE:
        extension = constants::VULKAN_SHADER_COMPUTE_EXTENSION;
        break;
    case ShaderStage::VERTEX:
        extension = constants::VULKAN_SHADER_VERTEX_EXTENSION;
        break;
    case ShaderStage::FRAGMENT:
        extension = constants::VULKAN_SHADER_FRAGMENT_EXTENSION;
        break;
    case ShaderStage::RAYTRACE_RAY_GEN:
        extension = constants::VULKAN_SHADER_RAYTRACE_RAY_GEN_EXTENSION;
        break;
    case ShaderStage::RAYTRACE_MISS:
        extension = constants::VULKAN_SHADER_RAYTRACE_MISS_EXTENSION;
        break;
    case ShaderStage::RAYTRACE_CLOSEST_HIT:
        extension = constants::VULKAN_SHADER_RAYTRACE_CLOSEST_HIT_EXTENSION;
        break;
    default:
        extension = "";
    }
    return utils::filesystem::combinePath(constants::VULKAN_SHADER_DIR_PATH,
                                          {name.c_str()}, extension);
}

bool kirana::viewport::vulkan::Shader::readShaderFile(
    const char *path, std::vector<uint32_t> *buffer)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return false;

    size_t fileSize = static_cast<size_t>(file.tellg());

    buffer->clear();
    buffer->resize(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read((char *)buffer->data(), static_cast<std::streamsize>(fileSize));

    return true;
}

kirana::viewport::vulkan::Shader::Shader(const Device *const device,
                                         const std::string &name)
    : m_name{name.empty() ? utils::constants::VULKAN_SHADER_DEFAULT_NAME
                          : name},
      m_device{device}
{
    for (int i = 0; i < static_cast<int>(ShaderStage::SHADER_STAGE_MAX); i++)
    {
        const auto stage = static_cast<ShaderStage>(i);
        const std::string path = getPathForShaderStage(name, stage);
        if (utils::filesystem::fileExists(path.c_str()))
        {
            std::vector<uint32_t> data;
            if (!readShaderFile(path.c_str(), &data))
            {
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error,
                                  ("Failed to read " + std::to_string(i) +
                                   " stage for shader: " + std::string(name))
                                      .c_str());
                return;
            }
            try
            {
                m_stages[stage] = m_device->current.createShaderModule(
                    vk::ShaderModuleCreateInfo(
                        {}, data.size() * sizeof(uint32_t), data.data()));
            }
            catch (...)
            {
                handleVulkanException();
            }
        }
    }
}

kirana::viewport::vulkan::Shader::~Shader()
{
    if (m_device)
    {
        for (const auto &s : m_stages)
            if (s.second)
                m_device->current.destroyShaderModule(s.second);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          (std::string(m_name) + " shader destroyed").c_str());
    }
}
