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


std::vector<kirana::viewport::vulkan::DescriptorData> kirana::viewport::vulkan::
    Shader::getDescriptors(DescriptorFrequency frequency,
                           scene::ShadingPipeline pipeline)
{
    switch (frequency)
    {
    case DescriptorFrequency::GLOBAL: {
        std::vector<DescriptorData> globalDescriptors(2);

        const vk::DescriptorType type =
            pipeline == scene::ShadingPipeline::RASTER
                ? vk::DescriptorType::eUniformBufferDynamic
                : vk::DescriptorType::eUniformBuffer;
        // Camera Descriptor
        vk::ShaderStageFlags stages = pipeline == scene::ShadingPipeline::RASTER
                                          ? vk::ShaderStageFlagBits::eVertex
                                          : vk::ShaderStageFlagBits::eRaygenKHR;
        globalDescriptors[0] = DescriptorData{0, type, stages};

        // World Descriptor
        stages = pipeline == scene::ShadingPipeline::RASTER
                     ? vk::ShaderStageFlagBits::eVertex |
                           vk::ShaderStageFlagBits::eFragment
                     : vk::ShaderStageFlagBits::eClosestHitKHR |
                           vk::ShaderStageFlagBits::eMissKHR |
                           vk::ShaderStageFlagBits::eAnyHitKHR;
        globalDescriptors[1] = DescriptorData{1, type, stages};

        return globalDescriptors;
    }
    case DescriptorFrequency::MATERIAL: {
        std::vector<DescriptorData> materialDescriptors(1);

        const vk::DescriptorType type =
            pipeline == scene::ShadingPipeline::RASTER
                ? vk::DescriptorType::eStorageBufferDynamic
                : vk::DescriptorType::eStorageBuffer;
        // Material Data Descriptor
        const vk::ShaderStageFlags stages =
            pipeline == scene::ShadingPipeline::RASTER
                ? vk::ShaderStageFlagBits::eFragment
                : vk::ShaderStageFlagBits::eClosestHitKHR |
                      vk::ShaderStageFlagBits::eAnyHitKHR;
        materialDescriptors[0] = DescriptorData{0, type, stages};

        return materialDescriptors;
    }
    case DescriptorFrequency::OBJECT: {
        std::vector<DescriptorData> objectDescriptors(1);

        const vk::DescriptorType type =
            pipeline == scene::ShadingPipeline::RASTER
                ? vk::DescriptorType::eStorageBufferDynamic
                : vk::DescriptorType::eStorageBuffer;
        // Material Data Descriptor
        const vk::ShaderStageFlags stages =
            pipeline == scene::ShadingPipeline::RASTER
                ? vk::ShaderStageFlagBits::eVertex
                : vk::ShaderStageFlagBits::eClosestHitKHR |
                      vk::ShaderStageFlagBits::eAnyHitKHR;
        objectDescriptors[0] = DescriptorData{0, type, stages};

        return objectDescriptors;
    }
    }
}

bool kirana::viewport::vulkan::Shader::createPipelineLayout(
    scene::ShadingPipeline pipeline)
{
    // TODO: Use Shader Reflection to build pipeline layout
    m_descLayouts.resize(3);
    m_descLayouts[0] = new DescriptorSetLayout(
        m_device, getDescriptors(DescriptorFrequency::GLOBAL, pipeline));
    m_descLayouts[1] = new DescriptorSetLayout(
        m_device, getDescriptors(DescriptorFrequency::MATERIAL, pipeline));
    m_descLayouts[2] = new DescriptorSetLayout(
        m_device, getDescriptors(DescriptorFrequency::OBJECT, pipeline));

    m_pushConstants.resize(1);
    switch (pipeline)
    {
    case scene::ShadingPipeline::RASTER: {
        m_pushConstants[0] = new PushConstant<vulkan::PushConstantRaster>(
            vulkan::PushConstantRaster{},
            vk::ShaderStageFlagBits::eVertex |
                vk::ShaderStageFlagBits::eFragment);
    }
    case scene::ShadingPipeline::RAYTRACE: {
        m_pushConstants[0] = new PushConstant<vulkan::PushConstantRaytrace>(
            vulkan::PushConstantRaytrace{},
            vk::ShaderStageFlagBits::eRaygenKHR |
                vk::ShaderStageFlagBits::eClosestHitKHR |
                vk::ShaderStageFlagBits::eAnyHitKHR);
    }
    }

    m_pipelineLayout =
        new PipelineLayout(m_device, m_descLayouts, m_pushConstants);

    return m_pipelineLayout->isInitialized;
}

kirana::viewport::vulkan::Shader::Shader(const Device *const device,
                                         const scene::ShaderData &shaderData)
    : m_isInitialized{false}, m_name{shaderData.name}, m_device{device}
{
    for (const auto &s : shaderData.stages)
    {
        const auto stage = static_cast<vk::ShaderStageFlagBits>(s.first);
        const std::string &path = s.second;
        std::vector<uint32_t> data;
        if (!readShaderFile(path.c_str(), &data))
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to read " +
                                  scene::shadingStageToString(s.first) +
                                  " stage for shader: " + m_name);
            return;
        }
        try
        {
            m_stages[stage] =
                m_device->current.createShaderModule(vk::ShaderModuleCreateInfo(
                    {}, data.size() * sizeof(uint32_t), data.data()));
        }
        catch (...)
        {
            handleVulkanException();
            return;
        }
    }
    m_isInitialized = createPipelineLayout(shaderData.pipeline);
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
        for (auto &p : m_pushConstants)
        {
            if (p)
            {
                delete p;
                p = nullptr;
            }
        }
        m_pushConstants.clear();
        for (auto &d : m_descLayouts)
        {
            if (d)
            {
                delete d;
                d = nullptr;
            }
        }
        m_descLayouts.clear();
        for (const auto &s : m_stages)
            if (s.second)
                m_device->current.destroyShaderModule(s.second);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          (std::string(m_name) + " shader destroyed").c_str());
    }
}
