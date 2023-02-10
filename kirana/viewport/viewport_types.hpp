#ifndef VIEWPORT_TYPES
#define VIEWPORT_TYPES

namespace kirana::viewport
{
enum class ShadingPipeline
{
    REALTIME = 0,
    RAYTRACE = 1,
    SHADING_MAX = 2,
};

enum class ShadingType
{
    BASIC = 0,
    UNLIT = 1,
    WIREFRAME = 2,
    BASIC_SHADED_WIREFRAME = 3,
    PBR = 4,
    AO = 5,
    SHADING_TYPE_MAX = 6
};

} // namespace kirana::viewport
#endif