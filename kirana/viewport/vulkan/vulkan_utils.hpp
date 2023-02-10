#ifndef KIRANA_VULKAN_UTILS_HPP
#define KIRANA_VULKAN_UTILS_HPP

#include <vulkan/vulkan.hpp>

#include <algorithm>
#include <vector>
#include <string>

#include <constants.h>
#include <logger.hpp>
#include <iostream>
namespace kirana::viewport::vulkan
{
namespace constants = kirana::utils::constants;
using kirana::utils::Logger;
using kirana::utils::LogSeverity;

/// Vector of necessary instance extensions when creating vulkan instance.
static const std::vector<const char *> REQUIRED_INSTANCE_EXTENSIONS{
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
/// Vector of necessary device extensions when selecting device.
static const std::vector<const char *> REQUIRED_DEVICE_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_RAY_QUERY_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME};
/// Vector of necessary validation numLayers for debugging.
static const std::vector<const char *> REQUIRED_VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation"};

static vk::PhysicalDeviceRayQueryFeaturesKHR DEVICE_RAY_QUERY_FEATURES{true};
static vk::PhysicalDeviceRayTracingPipelineFeaturesKHR
    DEVICE_RAYTRACE_PIPELINE_FEATURES{true, false, false, true, true};
static vk::PhysicalDeviceAccelerationStructureFeaturesKHR
    DEVICE_ACCEL_STRUCT_FEATURES{true, false, false, false, true};
static vk::PhysicalDeviceShaderDrawParametersFeatures
    DEVICE_SHADER_DRAW_PARAMS_FEATURES{true};
static vk::PhysicalDeviceBufferDeviceAddressFeatures
    DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES{true};
static vk::PhysicalDeviceShaderClockFeaturesKHR DEVICE_SHADER_CLOCK_FEATURES{
    true, true};

#ifndef VK_HANDLE_RESULT
#define VK_HANDLE_RESULT(f, err)                                               \
    {                                                                          \
        vk::Result result = f;                                                 \
        if (result != vk::Result::eSuccess)                                    \
        {                                                                      \
            Logger::get().log(constants::LOG_CHANNEL_VULKAN,                   \
                              LogSeverity::error, err);                        \
        }                                                                      \
    }
#endif

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
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
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
 * @return false if some or none of the required extensions are present.
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

static inline vk::PhysicalDeviceFeatures2 getRequiredDeviceFeatures()
{
    vk::PhysicalDeviceFeatures2 features;
    features.features.fillModeNonSolid = true;
    features.features.wideLines = true;
    features.features.logicOp = true;
    features.features.drawIndirectFirstInstance = true;
    features.features.tessellationShader = true;
    features.features.shaderInt64 = true;

    DEVICE_RAYTRACE_PIPELINE_FEATURES.pNext = &DEVICE_RAY_QUERY_FEATURES;
    DEVICE_ACCEL_STRUCT_FEATURES.pNext = &DEVICE_RAYTRACE_PIPELINE_FEATURES;
    DEVICE_SHADER_DRAW_PARAMS_FEATURES.pNext = &DEVICE_ACCEL_STRUCT_FEATURES;
    DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES.pNext =
        &DEVICE_SHADER_DRAW_PARAMS_FEATURES;
    DEVICE_SHADER_CLOCK_FEATURES.pNext = &DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    features.pNext = &DEVICE_SHADER_CLOCK_FEATURES;

    return features;
}

static bool hasRequiredDeviceFeatures(
    const vk::PhysicalDeviceFeatures2 &reqFeatures)
{
    if (!reqFeatures.features.fillModeNonSolid ||
        !reqFeatures.features.wideLines || !reqFeatures.features.logicOp ||
        !reqFeatures.features.drawIndirectFirstInstance ||
        !reqFeatures.features.tessellationShader ||
        !reqFeatures.features.shaderInt64)
        return false;

    auto *shaderClock =
        reinterpret_cast<vk::PhysicalDeviceShaderClockFeaturesKHR *>(
            reqFeatures.pNext);

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Clock Feature: " +
                          std::to_string(shaderClock->shaderDeviceClock));

    if (!shaderClock->shaderDeviceClock || !shaderClock->shaderSubgroupClock)
        return false;

    auto *bufferDeviceAddress =
        reinterpret_cast<vk::PhysicalDeviceBufferDeviceAddressFeatures *>(
            shaderClock->pNext);

    Logger::get().log(
        constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
        "Buffer Device Address Feature: " +
            std::to_string(bufferDeviceAddress->bufferDeviceAddress));

    if (!bufferDeviceAddress->bufferDeviceAddress)
        return false;

    auto *shaderDraw =
        reinterpret_cast<vk::PhysicalDeviceShaderDrawParametersFeatures *>(
            bufferDeviceAddress->pNext);

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Draw Feature: " +
                          std::to_string(shaderDraw->shaderDrawParameters));

    if (!shaderDraw->shaderDrawParameters)
        return false;

    auto *accelStruct =
        reinterpret_cast<vk::PhysicalDeviceAccelerationStructureFeaturesKHR *>(
            shaderDraw->pNext);

    Logger::get().log(
        constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
        "Acceleration Structure Feature: " +
            std::to_string(accelStruct->accelerationStructure) +
            ", Host Commands: " +
            std::to_string(accelStruct->accelerationStructureHostCommands) +
            ", Indirect Build: " +
            std::to_string(accelStruct->accelerationStructureIndirectBuild) +
            ", Update After Bind: " +
            std::to_string(
                accelStruct
                    ->descriptorBindingAccelerationStructureUpdateAfterBind));

    if (!accelStruct->accelerationStructure ||
        !accelStruct->descriptorBindingAccelerationStructureUpdateAfterBind)
        return false;

    auto *rayTracePipeline =
        reinterpret_cast<vk::PhysicalDeviceRayTracingPipelineFeaturesKHR *>(
            accelStruct->pNext);

    Logger::get().log(
        constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
        "Raytrace Pipeline Feature: " +
            std::to_string(rayTracePipeline->rayTracingPipeline) +
            ", Indirect Rays: " +
            std::to_string(
                rayTracePipeline->rayTracingPipelineTraceRaysIndirect) +
            ", Primitive Culling: " +
            std::to_string(rayTracePipeline->rayTraversalPrimitiveCulling));

    if (!rayTracePipeline->rayTracingPipeline ||
        !rayTracePipeline->rayTracingPipelineTraceRaysIndirect ||
        !rayTracePipeline->rayTraversalPrimitiveCulling)
        return false;

    auto *rayQuery = reinterpret_cast<vk::PhysicalDeviceRayQueryFeaturesKHR *>(
        rayTracePipeline->pNext);

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Ray Query Feature: " +
                          std::to_string(rayQuery->rayQuery));

    if (!rayQuery->rayQuery)
        return false;

    return true;
}

/**
 * @brief Find if vulkan supports the required validation numLayers.
 *
 * @return true if all required numLayers are present.
 * @return false if some or none of the numLayers are present.
 */
static bool hasRequiredValidationLayers()
{
    std::vector<vk::LayerProperties> layerProps =
        vk::enumerateInstanceLayerProperties();

    auto missingLayers = std::find_if(
        REQUIRED_VALIDATION_LAYERS.begin(), REQUIRED_VALIDATION_LAYERS.end(),
        [&layerProps](const char *vLayer) {
            return std::find_if(layerProps.begin(), layerProps.end(),
                                [&vLayer](const vk::LayerProperties &l) {
                                    return strcmp(vLayer, l.layerName);
                                }) == layerProps.end();
        });

    return missingLayers == REQUIRED_VALIDATION_LAYERS.end();
}

static std::string shaderStageToString(vk::ShaderStageFlagBits stage)
{
    switch (stage)
    {
    case vk::ShaderStageFlagBits::eVertex:
        return "VERTEX";
    case vk::ShaderStageFlagBits::eTessellationControl:
        return "TESSELLATION_CONTROL";
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
        return "TESSELLATION_EVALUATION";
    case vk::ShaderStageFlagBits::eGeometry:
        return "GEOMETRY";
    case vk::ShaderStageFlagBits::eFragment:
        return "FRAGMENT";
    case vk::ShaderStageFlagBits::eCompute:
        return "COMPUTE";
    case vk::ShaderStageFlagBits::eAllGraphics:
        return "ALL_GRAPHICS";
    case vk::ShaderStageFlagBits::eAll:
        return "ALL";
    case vk::ShaderStageFlagBits::eRaygenKHR:
        return "RAY_GEN";
    case vk::ShaderStageFlagBits::eAnyHitKHR:
        return "ANY_HIT";
    case vk::ShaderStageFlagBits::eClosestHitKHR:
        return "CLOSEST_HIT";
    case vk::ShaderStageFlagBits::eMissKHR:
        return "MISS";
    case vk::ShaderStageFlagBits::eIntersectionKHR:
        return "INTERSECTION";
    case vk::ShaderStageFlagBits::eCallableKHR:
        return "CALLABLE";
    default:
        return "UNKNOWN";
    }
}
} // namespace kirana::viewport::vulkan

#endif