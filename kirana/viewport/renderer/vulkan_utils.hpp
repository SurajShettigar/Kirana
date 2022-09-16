#ifndef VULKAN_UTILS_HPP
#define VULKAN_UTILS_HPP

#include <iostream>

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::renderer
{
/**
 * @brief Use it in catch(...) blocks of try-catch methods surrounding Vulkan
 * API calls.
 *
 */
static void handleVulkanException()
{
    try
    {
        throw;
    }
    catch (vk::SystemError &err)
    {
        std::cerr << "\033[31m [VULKAN EXCEPTION] " << err.what() << std::endl;
        std::cerr << "\033[0m";
    }
    catch (std::exception &err)
    {
        std::cerr << "\033[31m [EXCEPTION] " << err.what() << std::endl;
        std::cerr << "\033[0m";
    }
    catch (...)
    {
        std::cerr << "\033[31m [UNKNOWN EXCEPTION]" << std::endl;
        std::cerr << "\033[0m";
    }
}

/**
 * @brief Callback function for debug messenger.
 *
 * @param messageSeverity One of Verbose, Info, Warning or Error types.
 * @param messageType One of General, Performance or Validation types.
 * @param pCallbackData Data passed along by the debug messenger.
 * @param pUserData User data passed along by the debug messenger.
 * @return VKAPI_ATTR Returns VK_FALSE.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        std::cerr << "\033[32m [VALIDATION LAYER][VERBOSE] "
                  << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        std::cerr << "\033[32m [VALIDATION LAYER][INFO] "
                  << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        std::cerr << "\033[33m [VALIDATION LAYER][WARN] "
                  << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        std::cerr << "\033[31m [VALIDATION LAYER][ERROR] "
                  << pCallbackData->pMessage << std::endl;
        break;
    default:
        std::cerr << "\033[32m [VALIDATION LAYER] " << pCallbackData->pMessage
                  << std::endl;
        break;
    }
    std::cerr << "\033[0m";
    return VK_FALSE;
}
} // namespace kirana::viewport::renderer

#endif