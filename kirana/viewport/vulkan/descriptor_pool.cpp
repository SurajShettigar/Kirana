#include "descriptor_pool.hpp"

#include "device.hpp"
#include "descriptor_set.hpp"
#include "descriptor_set_layout.hpp"
#include "vulkan_utils.hpp"


kirana::viewport::vulkan::DescriptorPool::DescriptorPool(const Device *device)
    : m_isInitialized{false}, m_device{device}
{
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer,
                               constants::VULKAN_DESCRIPTOR_DEFAULT_POOL_SIZE),
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic,
                               constants::VULKAN_DESCRIPTOR_DEFAULT_POOL_SIZE),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer,
                               constants::VULKAN_DESCRIPTOR_DEFAULT_POOL_SIZE),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic,
                               constants::VULKAN_DESCRIPTOR_DEFAULT_POOL_SIZE),
        vk::DescriptorPoolSize(vk::DescriptorType::eAccelerationStructureKHR,
                               1),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1),
    };
    const vk::DescriptorPoolCreateInfo createInfo(
        {}, constants::VULKAN_DESCRIPTOR_SET_MAX_COUNT, poolSizes);

    try
    {
        m_current = m_device->current.createDescriptorPool(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Pool created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::DescriptorPool::~DescriptorPool()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyDescriptorPool(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Descriptor Pool destroyed");
        }
    }
}

bool kirana::viewport::vulkan::DescriptorPool::allocateDescriptorSet(
    const DescriptorSet *&descriptorSet,
    const DescriptorSetLayout *layout) const
{
    try
    {
        descriptorSet = new DescriptorSet(
            m_device,
            m_device->current.allocateDescriptorSets(
                vk::DescriptorSetAllocateInfo(m_current, layout->current))[0]);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Set allocated");
    }
    catch (...)
    {
        handleVulkanException();
        return false;
    }
    return true;
}

bool kirana::viewport::vulkan::DescriptorPool::allocateDescriptorSets(
    std::vector<const DescriptorSet **> sets,
    const std::vector<const DescriptorSetLayout *> &layouts) const
{
    std::vector<vk::DescriptorSetLayout> dLayouts(layouts.size());
    for (size_t i = 0; i < dLayouts.size(); i++)
        dLayouts[i] = layouts[i]->current;
    try
    {
        const std::vector<vk::DescriptorSet> tempSets =
            m_device->current.allocateDescriptorSets(
                vk::DescriptorSetAllocateInfo(m_current, dLayouts));

        for (size_t i = 0; i < layouts.size(); i++)
        {
            *sets[i] = new DescriptorSet(m_device, tempSets[i]);
        }
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Set allocated");
    }
    catch (...)
    {
        handleVulkanException();
        return false;
    }
    return true;
}