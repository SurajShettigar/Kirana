#include "instance.hpp"
#include "vulkan_utils.hpp"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

kirana::viewport::vulkan::Instance::Instance(
    const std::vector<const char *> &reqInstanceExtensions)
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Creating Vulkan Instance...");

    // Setup dynamic dispatcher (Used for extension functions).
    // Without dynamic dispatcher program will crash when using extension
    // functions.
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

    auto extensions = reqInstanceExtensions;
    if (constants::VULKAN_USE_VALIDATION_LAYERS)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if (!hasRequiredExtensions(vk::enumerateInstanceExtensionProperties(),
                               extensions)) // Check for extensions support
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Vulkan does not have the required extensions");
        m_isInitialized = false;
        return;
    }

    if (constants::VULKAN_USE_VALIDATION_LAYERS)
    {
        if (!hasRequiredValidationLayers()) // Check for validation layers.
        {
            Logger::get().log(
                constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                "Vulkan does not have the required validation layers");
            m_isInitialized = false;
            return;
        }
    }

    vk::ApplicationInfo appInfo(
        constants::APP_NAME,
        VK_MAKE_VERSION(constants::APP_VERSION[0], constants::APP_VERSION[1],
                        constants::APP_VERSION[2]),
        "Vulkan", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);

    // Create Vulkan Instance
    vk::InstanceCreateInfo createInfo({}, &appInfo, REQUIRED_VALIDATION_LAYERS,
                                      extensions);
    try
    {
        m_current = vk::createInstance(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Vulkan Instance created");

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_current);
#endif
        if (constants::VULKAN_USE_VALIDATION_LAYERS)
        {
            auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                m_current.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
            if (func == nullptr)
            {
                Logger::get().log(
                    constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                    "Failed to create Debug Messenger. Unable to Find "
                    "\"vkCreateDebugUtilsMessengerEXT\" function");
                m_isInitialized = false;
                return;
            }
            else
            {
                auto msgSeverity =
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
                auto msgTypes =
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

                auto debugMessengerCreateInfo =
                    vk::DebugUtilsMessengerCreateInfoEXT(
                        {}, msgSeverity, msgTypes, &debugCallback);

                m_debugMessenger = m_current.createDebugUtilsMessengerEXT(
                    debugMessengerCreateInfo, nullptr);
                m_isInitialized = true;
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::trace,
                                  "Vulkan Debugger initialized");
            }
        }
    }
    catch (...)
    {
        handleVulkanException();
        m_isInitialized = false;
        return;
    }
}

kirana::viewport::vulkan::Instance::~Instance()
{
    if (m_current)
    {
        if (m_debugMessenger)
        {
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                m_current.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
            if (func == nullptr)
            {
                Logger::get().log(
                    constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                    "Failed to destroy Debug Messenger. Unable to Find "
                    "\"vkDestroyDebugUtilsMessengerEXT\" function");
            }
            else
            {
                try
                {
                    m_current.destroyDebugUtilsMessengerEXT(m_debugMessenger,
                                                            nullptr);
                    Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                      LogSeverity::trace,
                                      "Vulkan Debugger destroyed");
                }
                catch (...)
                {
                    handleVulkanException();
                }
            }
        }
        m_current.destroy();
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Vulkan Instance destroyed");
    }
}