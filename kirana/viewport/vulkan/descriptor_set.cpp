#include "descriptor_set.hpp"

#include "descriptor_set_layout.hpp"
#include "texture.hpp"
#include "acceleration_structure.hpp"

#include "vulkan_utils.hpp"


bool kirana::viewport::vulkan::DescriptorSet::bindBuffer(
    const DescriptorBindingInfo &bindPoint, const AllocatedBuffer &buffer)
{
    if (!m_layout->containsBinding(bindPoint))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Descriptor Set layout binding does not match the "
                          "given binding point");
        return false;
    }
    m_writes.emplace_back(m_current, bindPoint.binding, 0,
                          bindPoint.descriptorCount, bindPoint.type, nullptr,
                          &buffer.descInfo);
    return true;
}

bool kirana::viewport::vulkan::DescriptorSet::bindTexture(
    const DescriptorBindingInfo &bindPoint, const Texture &texture)
{
    if (!m_layout->containsBinding(bindPoint))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Descriptor Set layout binding does not match the "
                          "given binding point");
        return false;
    }
    m_writes.emplace_back(m_current, bindPoint.binding, 0,
                          bindPoint.descriptorCount, bindPoint.type,
                          &texture.getDescriptorImageInfo());
    return true;
}

bool kirana::viewport::vulkan::DescriptorSet::bindTextures(
    const DescriptorBindingInfo &bindPoint,
    const std::vector<Texture *> &textures)
{
    if (!m_layout->containsBinding(bindPoint))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Descriptor Set layout binding does not match the "
                          "given binding point");
        return false;
    }
    if (textures.empty())
        return false;

    m_textureDescInfos.clear();
    m_textureDescInfos.resize(textures.size());
    for (int i = 0; i < textures.size(); i++)
        m_textureDescInfos[i] = textures[i]->getDescriptorImageInfo();
    m_writes.emplace_back(m_current, bindPoint.binding, 0,
                          static_cast<uint32_t>(textures.size()), bindPoint.type,
                          m_textureDescInfos.data());
    return true;
}

bool kirana::viewport::vulkan::DescriptorSet::bindAccelerationStructure(
    const DescriptorBindingInfo &bindPoint,
    const AccelerationStructure &accelStruct)
{
    if (!m_layout->containsBinding(bindPoint))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Descriptor Set layout binding does not match the "
                          "given binding point");
        return false;
    }
    m_accelStructWrites.emplace_back(accelStruct.getAccelerationStructure());
    m_writes.emplace_back(m_current, bindPoint.binding, 0,
                          bindPoint.descriptorCount, bindPoint.type, nullptr,
                          nullptr, nullptr, &m_accelStructWrites.back());
    return true;
}