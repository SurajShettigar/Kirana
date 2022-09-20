#ifndef VULKAN_UTILS_HPP
#define VULKAN_UTILS_HPP

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <limits>
#include <set>

#include <vulkan/vulkan.hpp>

#include <constants.h>
#include <logger.hpp>


namespace kirana::viewport::vulkan
{
namespace constants = kirana::utils::constants;
using kirana::utils::Logger;
using kirana::utils::LogSeverity;

static const std::vector<const char *> DEVICE_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};
static const std::vector<const char *> VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation"};

struct QueueFamilyIndices
{
    uint32_t graphics = std::numeric_limits<uint32_t>::max();
    uint32_t presentation = std::numeric_limits<uint32_t>::max();

    inline bool isGraphicsSupported() const
    {
        return graphics != std::numeric_limits<uint32_t>::max();
    }

    inline bool isPresentationSupported() const
    {
        return presentation != std::numeric_limits<uint32_t>::max();
    }

    inline bool isGraphicsAndPresentSame() const
    {
        return graphics == presentation;
    }

    // Set is used so that each value is unique. Graphics and presentation
    // queue family can refer to the same thing, so this step is necessary
    inline std::set<uint32_t> getIndices() const
    {
        return std::set<uint32_t>({ graphics, presentation });
    }
};

struct SwapchainSupportInfo
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR> presentModes;

    inline bool isSupported() const
    {
        return !surfaceFormats.empty() && !presentModes.empty();
    }
};

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
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::fatal,
                          err.what());
    }
    catch (std::exception &err)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::fatal,
                          err.what());
    }
    catch (...)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::fatal,
                          "[UNKNOWN EXCEPTION]");
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
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                          pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::warning,
                          pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          pCallbackData->pMessage);
        break;
    default:
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                          pCallbackData->pMessage);
        break;
    }
    return VK_FALSE;
}

/**
 * @brief Find if all the required extensions are present in the available list
 * of vulkan extensions.
 *
 * @param availExtensions vector of Vulkan Extension Properties.
 * @param reqExtensions vector of required extensions.
 * @return true if all required extensions are present.
 * @return false if some or none of the requrired extensions are present.
 */
static bool hasRequiredExtensions(
    const std::vector<vk::ExtensionProperties> &availExtensions,
    const std::vector<const char *> &reqExtensions)
{
    // For each required extension, check if it exists in the list of vulkan
    // extensions
    auto missingExtensions = std::find_if(
        reqExtensions.begin(), reqExtensions.end(),
        [&availExtensions](const char *reqE) {
            return std::find_if(availExtensions.begin(), availExtensions.end(),
                                [&reqE](const vk::ExtensionProperties &e) {
                                    return strcmp(reqE, e.extensionName.data());
                                }) == availExtensions.end();
        });

    // If missing extensions iterator has reached the end, it found all the
    // required extensions, otherwise there are some missing extensions
    return missingExtensions == reqExtensions.end();
}

/**
 * @brief Find if vulkan supports the required validation layers.
 *
 * @return true if all required layers are present.
 * @return false if some or none of the layers are present.
 */
static bool hasRequiredValidationLayers()
{
    std::vector<vk::LayerProperties> layerProps =
        vk::enumerateInstanceLayerProperties();

    auto missingLayers = std::find_if(
        VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end(),
        [&layerProps](const char *vLayer) {
            return std::find_if(layerProps.begin(), layerProps.end(),
                                [&vLayer](const vk::LayerProperties &l) {
                                    return strcmp(vLayer, l.layerName);
                                }) == layerProps.end();
        });

    return missingLayers == VALIDATION_LAYERS.end();
}
} // namespace kirana::viewport::vulkan

#endif