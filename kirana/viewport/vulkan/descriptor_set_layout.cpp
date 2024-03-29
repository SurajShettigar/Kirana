#include "descriptor_set_layout.hpp"

#include "device.hpp"
#include "vulkan_utils.hpp"

#include <algorithm>

kirana::viewport::vulkan::DescriptorSetLayout::DescriptorSetLayout(
    const Device *device, const std::vector<DescriptorBindingInfo> &bindings,
    bool dynamicDescriptors)
    : m_isInitialized{false}, m_device{device}, m_bindings{bindings},
      m_hasDynamicDescriptorBindings{dynamicDescriptors}
{
    std::vector<vk::DescriptorSetLayoutBinding> vkBindings(m_bindings.size());
    std::vector<vk::DescriptorBindingFlagsEXT> vkBindingFlags;
    for (size_t i = 0; i < m_bindings.size(); i++)
    {
        vkBindings[i] = vk::DescriptorSetLayoutBinding{
            m_bindings[i].binding, m_bindings[i].type,
            m_bindings[i].descriptorCount, m_bindings[i].stages};
        if (dynamicDescriptors)
        {
            vkBindingFlags.emplace_back(
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound |
                vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind);
            if (i == m_bindings.size() - 1)
                vkBindingFlags.back() |=
                    vk::DescriptorBindingFlagBitsEXT::eVariableDescriptorCount;
        }
    }

    const vk::DescriptorSetLayoutCreateFlags layoutFlags =
        dynamicDescriptors
            ? vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPoolEXT
            : vk::DescriptorSetLayoutCreateFlags{};

    vk::DescriptorSetLayoutCreateInfo createInfo(layoutFlags, vkBindings);
    const vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT
        bindingFlagsCreateInfoExt{vkBindingFlags};
    if (dynamicDescriptors)
        createInfo.pNext = &bindingFlagsCreateInfoExt;
    try
    {
        m_current = m_device->current.createDescriptorSetLayout(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Set Layout created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyDescriptorSetLayout(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Descriptor Set Layout destroyed");
        }
    }
}


bool kirana::viewport::vulkan::DescriptorSetLayout::containsBinding(
    const DescriptorBindingInfo &bindingInfo) const
{
    if (m_bindings.empty())
        return false;
    auto it = std::find_if(m_bindings.begin(), m_bindings.end(),
                           [&bindingInfo](const DescriptorBindingInfo &b) {
                               return b == bindingInfo;
                           });

    return it != m_bindings.end();
}


kirana::viewport::vulkan::DescriptorBindingInfo kirana::viewport::vulkan::
    DescriptorSetLayout::getBindingInfoForData(
        DescriptorBindingDataType dataType,
        vulkan::ShadingPipeline shadingPipeline)
{
    switch (dataType)
    {
    case DescriptorBindingDataType::CAMERA:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{DescriptorLayoutType::GLOBAL, 0,
                                           vk::DescriptorType::
                                               eUniformBufferDynamic,
                                           vk::ShaderStageFlagBits::eVertex}
                   : DescriptorBindingInfo{DescriptorLayoutType::GLOBAL, 0,
                                           vk::DescriptorType::eUniformBuffer,
                                           vk::ShaderStageFlagBits::eRaygenKHR};
    case DescriptorBindingDataType::WORLD:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{DescriptorLayoutType::GLOBAL, 1,
                                           vk::DescriptorType::
                                               eUniformBufferDynamic,
                                           vk::ShaderStageFlagBits::eVertex |
                                               vk::ShaderStageFlagBits::
                                                   eFragment}
                   : DescriptorBindingInfo{DescriptorLayoutType::GLOBAL, 1,
                                           vk::DescriptorType::eUniformBuffer,
                                           vk::ShaderStageFlagBits::eRaygenKHR};
    case DescriptorBindingDataType::RAYTRACE_ACCEL_STRUCT:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{}
                   : DescriptorBindingInfo{
                         DescriptorLayoutType::GLOBAL, 2,
                         vk::DescriptorType::eAccelerationStructureKHR,
                         vk::ShaderStageFlagBits::eRaygenKHR};
    case DescriptorBindingDataType::RAYTRACE_RENDER_TARGET:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{}
                   : DescriptorBindingInfo{DescriptorLayoutType::GLOBAL, 3,
                                           vk::DescriptorType::eStorageImage,
                                           vk::ShaderStageFlagBits::eRaygenKHR};
    case DescriptorBindingDataType::TEXTURE_DATA:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{DescriptorLayoutType::MATERIAL, 0,
                                           vk::DescriptorType::
                                               eCombinedImageSampler,
                                           vk::ShaderStageFlagBits::eFragment,
                                           utils::constants::
                                               VULKAN_DESCRIPTOR_DEFAULT_SAMPLED_IMAGES_SIZE}
                   : DescriptorBindingInfo{
                         DescriptorLayoutType::MATERIAL, 0,
                         vk::DescriptorType::eCombinedImageSampler,
                         vk::ShaderStageFlagBits::eRaygenKHR,
                         utils::constants::
                             VULKAN_DESCRIPTOR_DEFAULT_SAMPLED_IMAGES_SIZE};
    case DescriptorBindingDataType::OBJECT_DATA:
        return shadingPipeline == ShadingPipeline::RASTER
                   ? DescriptorBindingInfo{DescriptorLayoutType::OBJECT, 0,
                                           vk::DescriptorType::
                                               eStorageBufferDynamic,
                                           vk::ShaderStageFlagBits::eVertex, 0}
                   : DescriptorBindingInfo{DescriptorLayoutType::OBJECT, 0,
                                           vk::DescriptorType::eStorageBuffer,
                                           vk::ShaderStageFlagBits::eRaygenKHR};
    default:
        return DescriptorBindingInfo{};
    }
}

bool kirana::viewport::vulkan::DescriptorSetLayout::getDefaultDescriptorLayout(
    const Device *const device, DescriptorLayoutType layoutType,
    vulkan::ShadingPipeline pipeline, const DescriptorSetLayout *&layout)
{
    switch (layoutType)
    {
    case DescriptorLayoutType::GLOBAL: {
        std::vector<DescriptorBindingInfo> globalDescriptors;
        globalDescriptors.emplace_back(
            getBindingInfoForData(DescriptorBindingDataType::CAMERA, pipeline));
        globalDescriptors.emplace_back(
            getBindingInfoForData(DescriptorBindingDataType::WORLD, pipeline));

        if (pipeline == ShadingPipeline::RAYTRACE)
        {
            globalDescriptors.emplace_back(getBindingInfoForData(
                DescriptorBindingDataType::RAYTRACE_ACCEL_STRUCT, pipeline));
            globalDescriptors.emplace_back(getBindingInfoForData(
                DescriptorBindingDataType::RAYTRACE_RENDER_TARGET, pipeline));
        }

        layout = new DescriptorSetLayout(device, globalDescriptors);
        return layout->m_isInitialized;
    }
    case DescriptorLayoutType::MATERIAL: {
        layout = new DescriptorSetLayout(
            device,
            {getBindingInfoForData(DescriptorBindingDataType::TEXTURE_DATA,
                                   pipeline)},
            true);
        return layout->m_isInitialized;
    }
    case DescriptorLayoutType::OBJECT: {
        layout = new DescriptorSetLayout(
            device, {getBindingInfoForData(
                        DescriptorBindingDataType::OBJECT_DATA, pipeline)});
        return layout->m_isInitialized;
    }
    default:
        return false;
    }
}