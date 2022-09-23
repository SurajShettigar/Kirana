#include "shader.hpp"

#include <fstream>
#include <string>
#include "device.hpp"

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

kirana::viewport::vulkan::Shader::Shader(const Device *const device, const char *name)
    : m_isInitialized{false}, m_name{name}, m_device{device}
{
    std::string vShaderPath =
        std::string(constants::VULKAN_SHADER_DIR) + "/" + name + ".vert.spv";
    std::string fShaderPath =
        std::string(constants::VULKAN_SHADER_DIR) + "/" + name + ".frag.spv";

    std::vector<uint32_t> vShaderData;
    std::vector<uint32_t> fShaderData;

    if (!readShaderFile(vShaderPath.c_str(), &vShaderData))
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            ("Failed to read vertex shader for shader: " + std::string(name))
                .c_str());
        return;
    }
    if (!readShaderFile(fShaderPath.c_str(), &fShaderData))
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            ("Failed to read fragment shader for shader: " + std::string(name))
                .c_str());
        return;
    }

    try
    {
        m_vertex =
            m_device->current.createShaderModule(vk::ShaderModuleCreateInfo(
                {}, vShaderData.size() * sizeof(uint32_t), vShaderData.data()));
        m_fragment =
            m_device->current.createShaderModule(vk::ShaderModuleCreateInfo(
                {}, fShaderData.size() * sizeof(uint32_t), fShaderData.data()));

        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          (std::string(name) + " shader initialized").c_str());
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::Shader::~Shader()
{
    if (m_device)
    {
        if (m_fragment)
            m_device->current.destroyShaderModule(m_fragment);
        if (m_vertex)
            m_device->current.destroyShaderModule(m_vertex);

        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          (std::string(m_name) + " shader destroyed").c_str());
    }
}
