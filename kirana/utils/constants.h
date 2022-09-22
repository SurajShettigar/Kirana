#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace kirana::utils::constants
{

static const char *APP_NAME = "Kirana";
static const uint32_t APP_VERSION[3]{0, 1, 0};

static const char *LOG_CHANNEL_APPLICATION = "APPLICATION";
static const char *LOG_CHANNEL_VIEWPORT = "VIEWPORT";
static const char *LOG_CHANNEL_VULKAN = "VULKAN";
static const char *LOG_CHANNEL_SCENE = "SCENE";
static const char *LOG_CHANNEL_WINDOW = "WINDOW";
static const char *LOG_CHANNEL_UI = "UI";

#if DEBUG
static const bool VULKAN_USE_VALIDATION_LAYERS = true;
#else
static const bool VULKAN_USE_VALIDATION_LAYERS = false;
#endif
static const uint64_t VULKAN_FRAME_SYNC_TIMEOUT = 1000000000; // 1 second

} // namespace kirana::utils::constants

#endif