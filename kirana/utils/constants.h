#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>

namespace kirana::utils::constants
{

static const char *const APP_NAME = "Kirana";
static const uint32_t APP_VERSION[3]{0, 1, 0};

static const char *const LOG_CHANNEL_APPLICATION = "APPLICATION";
static const char *const LOG_CHANNEL_VIEWPORT = "VIEWPORT";
static const char *const LOG_CHANNEL_VULKAN = "VULKAN";
static const char *const LOG_CHANNEL_SCENE = "SCENE";
static const char *const LOG_CHANNEL_WINDOW = "WINDOW";
static const char *const LOG_CHANNEL_UI = "UI";

#if DEBUG
static const bool VULKAN_USE_VALIDATION_LAYERS = true;
#else
static const bool VULKAN_USE_VALIDATION_LAYERS = false;
#endif
static const uint64_t VULKAN_FRAME_SYNC_TIMEOUT = 1000000000; // 1 second
static const uint64_t VULKAN_COPY_BUFFER_WAIT_TIMEOUT =
    10000000000; // 10 seconds
static const uint16_t VULKAN_FRAME_OVERLAP_COUNT = 2;
static const uint16_t VULKAN_DESCRIPTOR_SET_MAX_COUNT = 10;
static const uint16_t VULKAN_DESCRIPTOR_DEFAULT_POOL_SIZE = 10;
static const uint64_t VULKAN_ACCELERATION_STRUCTURE_BATCH_SIZE_LIMIT =
    268435456; // 256 MB

static const char *const VULKAN_SHADER_COMPUTE_EXTENSION = ".comp.spv";
static const char *const VULKAN_SHADER_VERTEX_EXTENSION = ".vert.spv";
static const char *const VULKAN_SHADER_FRAGMENT_EXTENSION = ".frag.spv";
static const char *const VULKAN_SHADER_RAYTRACE_RAY_GEN_EXTENSION = ".rgen.spv";
static const char *const VULKAN_SHADER_RAYTRACE_MISS_EXTENSION = ".rmiss.spv";
static const char *const VULKAN_SHADER_RAYTRACE_MISS_SHADOW_EXTENSION =
    ".shadow.rmiss.spv";
static const char *const VULKAN_SHADER_RAYTRACE_CLOSEST_HIT_EXTENSION =
    ".rchit.spv";
static const char *const VULKAN_SHADER_MAIN_FUNC_NAME = "main";
static const char *const VULKAN_SHADER_DIR_PATH = SHADER_DIR;

static const char *const VULKAN_SHADER_DEFAULT_NAME = "Principled";
static const char *const VULKAN_SHADER_MAT_CAP_NAME = "MatCap";
static const char *const VULKAN_SHADER_WIREFRAME_NAME = "Wireframe";
static const char *const VULKAN_SHADER_GRID_NAME = "Grid";
static const char *const VULKAN_SHADER_OUTLINE_NAME = "Outline";
static const char *const VULKAN_SHADER_RAYTRACE_NAME = "Raytrace";

static const char *const DEFAULT_MATERIAL_MAT_CAP_NAME = "Default_MatCap_Mat";
static const char *const DEFAULT_MATERIAL_WIREFRAME_NAME =
    "Default_Wireframe_Mat";
static const char *const DEFAULT_MATERIAL_GRID_NAME = "Default_Grid_Mat";
static const char *const DEFAULT_MATERIAL_CAMERA_NAME = "Default_Camera_Mat";
static const char *const DEFAULT_MATERIAL_LIGHT_NAME = "Default_Light_Mat";
static const char *const DEFAULT_MATERIAL_OUTLINE_NAME = "Default_Outline_Mat";
static const char *const DEFAULT_MATERIAL_RAYTRACE_NAME =
    "Default_Raytrace_Mat";

static const char *const DEFAULT_SCENE_NAME = "Scene";
static const char *const DEFAULT_SCENE_MATERIAL_NAME = "Material";
static const char *const DEFAULT_SCENE_MATERIAL_SHADER_NAME =
    VULKAN_SHADER_DEFAULT_NAME;

static const char *const DATA_DIR_PATH = DATA_DIR;
//static const char *const DEFAULT_MODEL_NAME = "Quad_Model.fbx";
 static const char * const DEFAULT_MODEL_NAME = "Large_Model.fbx";

static const float VIEWPORT_SELECTED_OBJECT_OUTLINE_WIDTH = 0.025f;
static const std::array<float, 3> VIEWPORT_SELECTED_OBJECT_OUTLINE_COLOR = {
    1.0f, 1.0f, 1.0f};
static const float VIEWPORT_CAMERA_MOUSE_SENSITIVITY = 6.0f;
static const std::array<float, 3> VIEWPORT_CAMERA_DEFAULT_POSITION{0.0f, 4.0f,
                                                                   3.0f};
static const float VIEWPORT_CAMERA_DEFAULT_Z_OFFSET{3.0f};

} // namespace kirana::utils::constants

#endif
