#ifndef DESCRIPTOR_SET_HPP
#define DESCRIPTOR_SET_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class DescriptorSetLayout;
class Texture;
class AccelerationStructure;
class DescriptorSet
{
  private:
    vk::DescriptorSet m_current;
    const DescriptorSetLayout *m_layout = nullptr;
    mutable std::vector<vk::DescriptorImageInfo> m_textureDescInfos;
    mutable std::vector<vk::WriteDescriptorSetAccelerationStructureKHR>
        m_accelStructWrites;
    mutable std::vector<vk::WriteDescriptorSet> m_writes;

  public:
    DescriptorSet() = default;
    explicit DescriptorSet(vk::DescriptorSet descriptorSet,
                           const DescriptorSetLayout *layout)
        : m_current{descriptorSet}, m_layout{layout} {};
    ~DescriptorSet() = default;

    DescriptorSet(const DescriptorSet &descriptorSet)
    {
        if (this != &descriptorSet)
        {
            m_current = descriptorSet.m_current;
            m_layout = descriptorSet.m_layout;
            m_accelStructWrites = descriptorSet.m_accelStructWrites;
            m_writes = descriptorSet.m_writes;
        }
    }
    DescriptorSet &operator=(const DescriptorSet &descriptorSet)
    {
        if (this != &descriptorSet)
        {
            m_current = descriptorSet.m_current;
            m_layout = descriptorSet.m_layout;
            m_accelStructWrites = descriptorSet.m_accelStructWrites;
            m_writes = descriptorSet.m_writes;
        }
        return *this;
    }

    explicit operator vk::DescriptorSet() const
    {
        return m_current;
    }

    const vk::DescriptorSet &current = m_current;

    [[nodiscard]] inline const DescriptorSetLayout &getLayout() const
    {
        return *m_layout;
    }

    [[nodiscard]] inline const std::vector<vk::WriteDescriptorSet> &getWrites()
        const
    {
        return m_writes;
    }

    inline void clearWrites() const
    {
        m_writes.clear();
        m_accelStructWrites.clear();
        m_textureDescInfos.clear();
    }

    bool bindBuffer(const DescriptorBindingInfo &bindPoint,
                    const AllocatedBuffer &buffer);
    bool bindTexture(const DescriptorBindingInfo &bindPoint,
                   const Texture &texture);
    bool bindTextures(const DescriptorBindingInfo &bindPoint, const std::vector<Texture *> &textures);
    bool bindAccelerationStructure(const DescriptorBindingInfo &bindPoint,
                                   const AccelerationStructure &accelStruct);
};
} // namespace kirana::viewport::vulkan


#endif
