#include "shader.hpp"
#include "device.hpp"
#include "descriptor_set_layout.hpp"
#include "push_constant.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"

#include <material_types.hpp>
#include <fstream>
#include <string>
#include <file_system.hpp>

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
                                         const scene::ShaderData &shaderData)
    : m_isInitialized{false}, m_device{device}, m_name{shaderData.name},
      m_shadingPipeline{
          static_cast<vulkan::ShadingPipeline>(shaderData.pipeline)}
{
    for (const auto &s : shaderData.stages)
    {
        const auto stage = static_cast<vk::ShaderStageFlagBits>(s.first);
        m_stages[stage].resize(s.second.size());
        for (int i = 0; i < s.second.size(); i++)
        {
            const std::string &path = s.second[i];
            std::vector<uint32_t> data;
            if (!readShaderFile(path.c_str(), &data))
            {
                Logger::get().log(
                    constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                    "Failed to read " + scene::shadingStageToString(s.first) +
                        " stage for shader: " + m_name);
                return;
            }
            try
            {
                m_stages[stage][i] = m_device->current.createShaderModule(
                    vk::ShaderModuleCreateInfo(
                        {}, data.size() * sizeof(uint32_t), data.data()));

                m_device->setDebugObjectName(m_stages.at(stage)[i],
                                             "ShaderModule_" + m_name + "_" +
                                                 std::to_string(i) + "_" +
                                                 shaderStageToString(stage));
            }
            catch (...)
            {
                handleVulkanException();
                return;
            }
        }
    }

    // TODO: Use Shader Reflection to build pipeline layout
    m_isInitialized = PipelineLayout::getDefaultPipelineLayout(
        m_device, m_shadingPipeline, m_pipelineLayout);
}

kirana::viewport::vulkan::Shader::~Shader()
{
    if (m_device)
    {
        if (m_pipelineLayout)
        {
            delete m_pipelineLayout;
            m_pipelineLayout = nullptr;
        }
        for (const auto &s : m_stages)
            for (const auto &m : s.second)
                if (m)
                    m_device->current.destroyShaderModule(m);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          (std::string(m_name) + " shader destroyed").c_str());
    }
}
