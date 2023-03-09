#ifndef MATERIAL_TYPES_HPP
#define MATERIAL_TYPES_HPP

#include <array>
#include <vector>
#include <any>
#include <string>
#include <utility>
#include "scene_types.hpp"

namespace kirana::scene
{

enum class ShadingPipeline
{
    RASTER = 0,
    RAYTRACE = 1,
    SHADING_PIPELINE_MAX = 2
};

enum class ShadingStage
{
    NONE = 0x00000000,
    VERTEX = 0x00000001,
    TESSELLATION_CONTROL = 0x00000002,
    TESSELLATION_EVALUATION = 0x00000004,
    GEOMETRY = 0x00000008,
    FRAGMENT = 0x00000010,
    COMPUTE = 0x00000020,
    ALL_GRAPHICS = 0x0000001F,
    ALL = 0x7FFFFFFF,
    RAY_GEN = 0x00000100,
    ANY_HIT = 0x00000200,
    CLOSEST_HIT = 0x00000400,
    MISS = 0x00000800,
    INTERSECTION = 0x00001000,
    CALLABLE = 0x00002000,
};

static std::string shadingStageToString(ShadingStage stage)
{
    switch (stage)
    {
    case ShadingStage::NONE:
        return "NONE";
    case ShadingStage::VERTEX:
        return "VERTEX";
    case ShadingStage::TESSELLATION_CONTROL:
        return "TESSELLATION_CONTROL";
    case ShadingStage::TESSELLATION_EVALUATION:
        return "TESSELLATION_EVALUATION";
    case ShadingStage::GEOMETRY:
        return "GEOMETRY";
    case ShadingStage::FRAGMENT:
        return "FRAGMENT";
    case ShadingStage::COMPUTE:
        return "COMPUTE";
    case ShadingStage::ALL_GRAPHICS:
        return "ALL_GRAPHICS";
    case ShadingStage::ALL:
        return "ALL";
    case ShadingStage::RAY_GEN:
        return "RAY_GEN";
    case ShadingStage::ANY_HIT:
        return "ANY_HIT";
    case ShadingStage::CLOSEST_HIT:
        return "CLOSEST_HIT";
    case ShadingStage::MISS:
        return "MISS";
    case ShadingStage::INTERSECTION:
        return "INTERSECTION";
    case ShadingStage::CALLABLE:
        return "CALLABLE";
    default:
        return "UNKNOWN";
    }
}

static const std::unordered_map<std::string, ShadingStage>
    SHADING_EXTENSION_STAGE_TABLE{
        {"vert", ShadingStage::VERTEX},
        {"tesc", ShadingStage::TESSELLATION_CONTROL},
        {"tese", ShadingStage::TESSELLATION_EVALUATION},
        {"geom", ShadingStage::GEOMETRY},
        {"frag", ShadingStage::FRAGMENT},
        {"comp", ShadingStage::COMPUTE},
        {"rgen", ShadingStage::RAY_GEN},
        {"rahit", ShadingStage::ANY_HIT},
        {"rchit", ShadingStage::CLOSEST_HIT},
        {"rmiss", ShadingStage::MISS},
        {"rint", ShadingStage::INTERSECTION},
        {"rcall", ShadingStage::CALLABLE},
    };

struct ShaderData
{
    std::string name;
    ShadingPipeline pipeline;
    std::unordered_map<ShadingStage, std::vector<std::string>> stages;
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
    WIREFRAME = 2,
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
    MaterialDataBase() = default;
    virtual ~MaterialDataBase() = default;

    [[nodiscard]] inline virtual size_t size() const
    {
        return sizeof(*this);
    };
    inline virtual void *data()
    {
        return this;
    };

    inline virtual const void *data() const
    {
        return this;
    };
};

struct RasterPipelineData : MaterialDataBase
{
    CullMode cull = CullMode::BACK;
    SurfaceType surfaceType = SurfaceType::OPAQUE;
    bool enableDepth = true;
    bool writeDepth = true;
    CompareOperation depthCompareOp = CompareOperation::LESS_OR_EQUAL;
    StencilProperties stencil;
    std::vector<VertexInfo> vertexAttributeInfo = Vertex::getVertexInfo();

    explicit RasterPipelineData(
        CullMode cull = CullMode::BACK,
        SurfaceType surfaceType = SurfaceType::OPAQUE, bool enableDepth = true,
        bool writeDepth = true,
        CompareOperation depthCompareOp = CompareOperation::LESS_OR_EQUAL,
        StencilProperties stencil = {},
        const std::vector<VertexInfo> &vertexAttributeInfo =
            Vertex::getVertexInfo())
        : MaterialDataBase(), cull{cull}, surfaceType{surfaceType},
          enableDepth{enableDepth}, writeDepth{writeDepth},
          depthCompareOp{depthCompareOp}, stencil{stencil},
          vertexAttributeInfo{vertexAttributeInfo}
    {
    }

    [[nodiscard]] inline size_t size() const override
    {
        return sizeof(*this);
    }
    inline void *data() override
    {
        return this;
    }

    inline const void *data() const override
    {
        return this;
    };
};

struct RaytracePipelineData : MaterialDataBase
{
    CullMode cull = CullMode::NONE;
    VertexInfo vertexInfo = Vertex::getLargestVertexInfo();
    uint32_t maxRecursionDepth = 2;

    explicit RaytracePipelineData(
        CullMode cull = CullMode::NONE,
        VertexInfo vertexInfo = Vertex::getLargestVertexInfo(),
        uint32_t maxRecursionDepth = 2)
        : MaterialDataBase(), cull{cull}, vertexInfo{vertexInfo},
          maxRecursionDepth{maxRecursionDepth}
    {
    }

    [[nodiscard]] inline size_t size() const override
    {
        return sizeof(*this);
    }
    inline void *data() override
    {
        return this;
    }

    inline const void *data() const override
    {
        return this;
    };
};

enum class MaterialParameterType
{
    BOOL = 0,
    INT = 1,
    UINT = 2,
    FLOAT = 3,
    TEX_1D = 4,
    TEX_2D = 5,
    TEX_3D = 6,
    INT64 = 7,
    UINT64 = 8,
    DOUBLE = 9,
    VEC_2 = 10,
    VEC_3 = 11,
    VEC_4 = 12,
    MAT_2x2 = 13,
    MAT_3x3 = 14,
    MAT_3x4 = 15,
    MAT_4x4 = 16,
};

struct MaterialParameter
{
    std::string id;
    MaterialParameterType type;
    std::any value;
};

// TODO: Use constant parameter names.

static const std::vector<MaterialParameter>
    DEFAULT_BASIC_SHADED_MATERIAL_PARAMETERS{
        {MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.65f, 0.65f, 0.65f, 1.0f)}}};

static const std::vector<MaterialParameter> DEFAULT_OUTLINE_MATERIAL_PARAMETERS{
    {MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                       math::Vector4(1.0f, 1.0f, 0.0f, 1.0f)}},
    {MaterialParameter{"_Thickness", MaterialParameterType::FLOAT, 0.02f}}};

static const std::vector<MaterialParameter>
    DEFAULT_WIREFRAME_MATERIAL_PARAMETERS{
        {MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.0f, 0.0f, 0.0f, 1.0f)}}};

static const std::vector<MaterialParameter>
    DEFAULT_BASIC_SHADED_WIREFRAME_MATERIAL_PARAMETERS{
        {MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.65f, 0.65f, 0.65f, 1.0f)}},
        {MaterialParameter{"_WireframeColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.0f, 0.0f, 0.0f, 1.0f)}}};

static const std::vector<MaterialParameter>
    DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS{
        {MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(1.0f, 1.0f, 1.0f, 1.0f)}},
        {MaterialParameter{"_SubSurface", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_Metallic", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_Specular", MaterialParameterType::FLOAT, 0.5f}},
        {MaterialParameter{"_SpecularTint", MaterialParameterType::FLOAT,
                           0.0f}},
        {MaterialParameter{"_Roughness", MaterialParameterType::FLOAT, 0.5f}},
        {MaterialParameter{"_Anisotropic", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_Sheen", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_SheenTint", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_ClearCoat", MaterialParameterType::FLOAT, 0.0f}},
        {MaterialParameter{"_ClearCoatGloss", MaterialParameterType::FLOAT,
                           0.0f}},
        {MaterialParameter{"_Transmission", MaterialParameterType::FLOAT,
                           0.0f}},
        {MaterialParameter{"_Ior", MaterialParameterType::FLOAT, 1.0f}},
        {MaterialParameter{"_EmissiveColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.0f, 0.0f, 0.0f, 1.0f)}},
        {MaterialParameter{"_EmissiveIntensity", MaterialParameterType::FLOAT,
                           1.0f}},
        {MaterialParameter{"_BaseMap", MaterialParameterType::TEX_2D, -1}},
        {MaterialParameter{"_EmissiveMap", MaterialParameterType::TEX_2D, -1}},
        {MaterialParameter{"_NormalMap", MaterialParameterType::TEX_2D,
                           -1}}};

} // namespace kirana::scene

#endif