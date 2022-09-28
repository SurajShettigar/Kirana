#include "allocator.hpp"

#include "instance.hpp"
#include "device.hpp"

#include "vulkan_utils.hpp"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.hpp>

kirana::viewport::vulkan::Allocator::Allocator(const Instance *instance,
                                               const Device *device)
    : m_isInitialized{false}, m_instance{instance}, m_device{device}
{
    try
    {
        vma::AllocatorCreateInfo createInfo({}, m_device->gpu,
                                            m_device->current);
        createInfo.setInstance(m_instance->current);
        m_current =
            std::make_unique<vma::Allocator>(vma::createAllocator(createInfo));
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Allocator created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::Allocator::~Allocator()
{
    if (m_current)
        m_current->destroy();

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                      "Allocator destroyed");
}