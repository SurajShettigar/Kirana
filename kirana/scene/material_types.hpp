#ifndef MATERIAL_TYPES_HPP
#define MATERIAL_TYPES_HPP

#include <unordered_map>
#include <string>
#include "scene_types.hpp"

namespace kirana::scene
{

enum class ShadingPipeline
{
    RASTER = 0,
    RAYTRACE = 1,
    COMPUTE = 2,
};

enum class ShadingType
{
    BASIC_SHADED = 0,
    UNLIT = 1,
    WIREFRAME = 2,
    BASIC_SHADED_WIREFRAME = 3,
    SHADED = 4,
    AO = 5
};

enum class CullMode
{
    NONE = 0,
    FRONT = 1,
    BACK = 2,
    BOTH = 3
};

enum class SurfaceType
{
    OPAQUE = 0,
    TRANSPARENT = 1,
};

enum class CompareOperation
{
    NEVER = 0,
    LESS = 1,
    EQUAL = 2,
    LESS_OR_EQUAL = 3,
    GREATER = 4,
    NOT_EQUAL = 5,
    GREATER_OR_EQUAL = 6,
    ALWAYS = 7
};

enum class StencilOperation
{
    KEEP = 0,
    ZERO = 1,
    REPLACE = 2,
    INCREMENT_AND_CLAMP = 3,
    DECREMENT_AND_CLAMP = 4,
    INVERT = 5,
    INCREMENT_AND_WRAP = 6,
    DECREMENT_AND_WRAP = 7
};

struct StencilProperties
{
    bool enableTest = false;
    CompareOperation compareOp = CompareOperation::ALWAYS;
    StencilOperation failOp = StencilOperation::REPLACE;
    StencilOperation depthFailOp = StencilOperation::REPLACE;
    StencilOperation passOp = StencilOperation::REPLACE;
    uint32_t reference = 1;
};

struct MaterialDataBase
{
};

struct RasterPipelineData : MaterialDataBase
{
    std::vector<VertexInfo> vertexAttributeInfo = Vertex::getVertexInfo();
    CullMode cull = CullMode::BACK;
    SurfaceType surfaceType = SurfaceType::OPAQUE;
    bool enableDepth = true;
    bool writeDepth = true;
    CompareOperation depthCompareOp = CompareOperation::LESS_OR_EQUAL;
    StencilProperties stencil;
};

struct RaytracePipelineData : MaterialDataBase
{
    VertexInfo vertexInfo = Vertex::getOverallVertexInfo();
    CullMode cull = CullMode::NONE;
};

struct SingleShadedMaterialData : MaterialDataBase
{
    math::Vector4 color;
};

struct OutlineMaterialData : MaterialDataBase
{
    math::Vector4 color;
    float thickness;
};

struct WireframeMaterialData : MaterialDataBase
{
    math::Vector4 color;
};

struct SingleShadedWireframeMaterialData : MaterialDataBase
{
    math::Vector4 color;
    math::Vector4 wireframeColor;
};

struct PrincipledMaterialData : MaterialDataBase
{
    math::Vector4 baseColor;
    float subSurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
    float transmission;
    float ior;
};

struct MaterialProperties
{
    RasterPipelineData rasterData;
    RasterPipelineData raytraceData;
    std::unique_ptr<MaterialDataBase> materialData;
};

} // namespace kirana::scene

#endif