#ifndef VIEWPORT_TYPES
#define VIEWPORT_TYPES

namespace kirana::viewport
{
enum class Shading
{
    BASIC = 0,
    WIREFRAME = 1,
    REALTIME_PBR = 2,
    RAYTRACE_PBR = 3
};
} // namespace kirana::viewport
#endif