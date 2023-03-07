#include "texture_sampler.hpp"

#include "device.hpp"
#include "vulkan_utils.hpp"

#include <math_utils.hpp>
#include <utility>


kirana::viewport::vulkan::TextureSampler::TextureSampler(
    const Device *device, Properties properties)
    : m_isInitialized{false}, m_device{device}, m_properties{std::move(properties)}
{
    // Check for limits
    const float maxLODBias =
        m_device->gpu.getProperties().limits.maxSamplerLodBias;
    m_properties.mipLODBias =
        math::clampf(m_properties.mipLODBias, 0.0f, maxLODBias);

    const float maxAnisotropicLevel =
        m_device->gpu.getProperties().limits.maxSamplerAnisotropy;
    m_properties.anisotropicLevel =
        math::clampf(m_properties.anisotropicLevel, 0.0f, maxAnisotropicLevel);

    vk::SamplerCreateInfo createInfo{
        vk::SamplerCreateFlags{},
        static_cast<vk::Filter>(m_properties.magFilter),
        static_cast<vk::Filter>(m_properties.minFilter),
        static_cast<vk::SamplerMipmapMode>(m_properties.mipMapFilter),
        static_cast<vk::SamplerAddressMode>(m_properties.uvwWrapMode[0]),
        static_cast<vk::SamplerAddressMode>(m_properties.uvwWrapMode[1]),
        static_cast<vk::SamplerAddressMode>(m_properties.uvwWrapMode[2]),
        m_properties.mipLODBias,
        m_properties.enableAnisotropicFiltering,
        m_properties.anisotropicLevel};
    createInfo.borderColor =
        static_cast<vk::BorderColor>(m_properties.borderColor);

    try
    {
        m_current = m_device->current.createSampler(createInfo);
        m_isInitialized = true;
    }
    catch (...)
    {
        vulkan::handleVulkanException();
    }
}

kirana::viewport::vulkan::TextureSampler::~TextureSampler()
{
    if (m_device)
    {
        if (m_current)
            m_device->current.destroySampler(m_current);
    }
}