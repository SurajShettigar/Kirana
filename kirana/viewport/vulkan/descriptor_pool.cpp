#include "descriptor_pool.hpp"

#include "device.hpp"
#include "descriptor_set.hpp"
#include "descriptor_set_layout.hpp"
#include "vulkan_utils.hpp"


kirana::viewport::vulkan::DescriptorPool::DescriptorPool(const Device *device)
    : m_isInitialized{false}, m_device{device}
{
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
            constants::VULKAN_DESCRIPTOR_DEFAULT_UNIFORM_BUFFER_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBufferDynamic,
            constants::VULKAN_DESCRIPTOR_DEFAULT_UNIFORM_BUFFER_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eStorageBuffer,
            constants::VULKAN_DESCRIPTOR_DEFAULT_STORAGE_BUFFER_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eStorageBufferDynamic,
            constants::VULKAN_DESCRIPTOR_DEFAULT_STORAGE_BUFFER_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eAccelerationStructureKHR,
            constants::VULKAN_DESCRIPTOR_DEFAULT_ACCEL_STRUCT_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eStorageImage,
            constants::VULKAN_DESCRIPTOR_DEFAULT_STORAGE_IMAGE_POOL_SIZE),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eSampledImage,
            constants::VULKAN_DESCRIPTOR_DEFAULT_SAMPLED_IMAGES_SIZE),
    };

    const uint32_t maxSets =
        2 * constants::VULKAN_DESCRIPTOR_DEFAULT_UNIFORM_BUFFER_POOL_SIZE +
        2 * constants::VULKAN_DESCRIPTOR_DEFAULT_STORAGE_BUFFER_POOL_SIZE +
        constants::VULKAN_DESCRIPTOR_DEFAULT_ACCEL_STRUCT_POOL_SIZE +
        constants::VULKAN_DESCRIPTOR_DEFAULT_STORAGE_IMAGE_POOL_SIZE +
        constants::VULKAN_DESCRIPTOR_DEFAULT_SAMPLED_IMAGES_SIZE;

    const vk::DescriptorPoolCreateInfo createInfo(
        {vk::DescriptorPoolCreateFlagBits::eUpdateAfterBindEXT}, maxSets,
        poolSizes);

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
    const DescriptorSetLayout *layout, DescriptorSet *set) const
{
    try
    {
        vk::DescriptorSetAllocateInfo allocateInfo{m_current, layout->current};
        vk::DescriptorSetVariableDescriptorCountAllocateInfo varAllocInfo{
            layout->getBindings().back().descriptorCount};
        if (layout->hasDynamicDescriptorBindings)
            allocateInfo.pNext = &varAllocInfo;

        const vk::DescriptorSet vkSet =
            m_device->current.allocateDescriptorSets(allocateInfo)[0];
        *set = DescriptorSet(vkSet, layout);
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
    const std::vector<const DescriptorSetLayout *> &layouts,
    std::vector<DescriptorSet> *sets) const
{
    std::vector<vk::DescriptorSetLayout> dLayouts(layouts.size());
    std::vector<uint32_t> variableDescCounts(layouts.size());
    for (size_t i = 0; i < dLayouts.size(); i++)
    {
        dLayouts[i] = layouts[i]->current;
        variableDescCounts[i] =
            layouts[i]->getBindings().back().descriptorCount;
    }
    try
    {
        vk::DescriptorSetAllocateInfo allocateInfo{m_current, dLayouts};
        vk::DescriptorSetVariableDescriptorCountAllocateInfo varAllocInfo{
            variableDescCounts};
        allocateInfo.pNext = &varAllocInfo;

        const std::vector<vk::DescriptorSet> tempSets =
            m_device->current.allocateDescriptorSets(allocateInfo);

        for (size_t i = 0; i < layouts.size(); i++)
        {
            (*sets)[i] = DescriptorSet(tempSets[i], layouts[i]);
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

void kirana::viewport::vulkan::DescriptorPool::writeDescriptorSet(
    const DescriptorSet &set) const
{
    m_device->current.updateDescriptorSets(set.getWrites(), {});
    set.clearWrites();
}