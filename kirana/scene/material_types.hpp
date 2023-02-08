#ifndef MATERIAL_TYPES_HPP
#define MATERIAL_TYPES_HPP

#include <unordered_map>
#include <string>
#include <utility>
#include <iostream>
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
    }
}

static const std::unordered_map<std::string, ShadingStage>
    SHADING_EXTENSION_STAGE_TABLE{
        {".vert.spv", ShadingStage::VERTEX},
        {".tesc.spv", ShadingStage::TESSELLATION_CONTROL},
        {".tese.spv", ShadingStage::TESSELLATION_EVALUATION},
        {".geom.spv", ShadingStage::GEOMETRY},
        {".frag.spv", ShadingStage::FRAGMENT},
        {".comp.spv", ShadingStage::COMPUTE},
        {".rgen.spv", ShadingStage::RAY_GEN},
        {".rahit.spv", ShadingStage::ANY_HIT},
        {".rchit.spv", ShadingStage::CLOSEST_HIT},
        {".rmiss.spv", ShadingStage::MISS},
        {".rint.spv", ShadingStage::INTERSECTION},
        {".rcall.spv", ShadingStage::CALLABLE},
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

    explicit RaytracePipelineData(CullMode cull = CullMode::NONE,
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

struct BasicShadedMaterialData : MaterialDataBase
{
    math::Vector4 color = math::Vector4::ONE;

    explicit BasicShadedMaterialData(const math::Vector4 &color = math::Vector4::ONE)
        : color{color}
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

struct OutlineMaterialData : MaterialDataBase
{
    math::Vector4 color = math::Vector4::ONE;
    alignas(4) float thickness = 0.01f;

    explicit OutlineMaterialData(const math::Vector4 &color = math::Vector4::ONE,
                        float thickness = 0.01f)
        : color{color}, thickness{thickness}
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

struct WireframeMaterialData : MaterialDataBase
{
    math::Vector4 color = math::Vector4::ONE;

    explicit WireframeMaterialData(const math::Vector4 &color = math::Vector4::ONE)
        : color{color}
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

struct SingleShadedWireframeMaterialData : MaterialDataBase
{
    math::Vector4 color = math::Vector4::ONE;
    math::Vector4 wireframeColor = math::Vector4::ONE;

    explicit SingleShadedWireframeMaterialData(
        const math::Vector4 &color = math::Vector4::ONE,
        const math::Vector4 &wireframeColor = math::Vector4::ONE)
        : color{color}, wireframeColor{wireframeColor}
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

struct PrincipledMaterialData : MaterialDataBase
{
    math::Vector4 baseColor = math::Vector4::ONE;
    alignas(4) float subSurface = 0.0f;
    alignas(4) float metallic = 0.0f;
    alignas(4) float specular = 0.0f;
    alignas(4) float specularTint = 0.0f;
    alignas(4) float roughness = 0.5f;
    alignas(4) float anisotropic = 0.0f;
    alignas(4) float sheen = 0.0f;
    alignas(4) float sheenTint = 0.0f;
    alignas(4) float clearCoat = 0.0f;
    alignas(4) float clearCoatGloss = 0.0f;
    alignas(4) float transmission = 0.0f;
    alignas(4) float ior = 1.0f;

    explicit PrincipledMaterialData(const math::Vector4 &baseColor = math::Vector4::ONE,
                           float subSurface = 0.0f, float metallic = 0.0f,
                           float specular = 0.0f, float specularTint = 0.0f,
                           float roughness = 0.5f, float anisotropic = 0.0f,
                           float sheen = 0.0f, float sheenTint = 0.0f,
                           float clearCoat = 0.0f, float clearCoatGloss = 0.0f,
                           float transmission = 0.0f, float ior = 1.0f)
        : baseColor{baseColor}, subSurface{subSurface}, metallic{metallic},
          specular{specular}, specularTint{specularTint}, roughness{roughness},
          anisotropic{anisotropic}, sheen{sheen}, sheenTint{sheenTint},
          clearCoat{clearCoat}, clearCoatGloss{clearCoatGloss},
          transmission{transmission}, ior{ior}
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

struct MaterialProperties
{
    RasterPipelineData rasterData{};
    RaytracePipelineData raytraceData{};
    std::unique_ptr<MaterialDataBase> materialData = nullptr;

    MaterialProperties() = default;

    MaterialProperties(RasterPipelineData rasterData,
                       RaytracePipelineData raytraceData,
                       std::unique_ptr<MaterialDataBase> &&materialData)
        : rasterData{std::move(rasterData)}, raytraceData{std::move(
                                                 raytraceData)},
          materialData{materialData != nullptr ? std::move(materialData)
                                               : nullptr}
    {
    }

    ~MaterialProperties() = default;

    MaterialProperties(const MaterialProperties &properties)
    {
        if (this != &properties)
        {
            rasterData = properties.rasterData;
            raytraceData = properties.raytraceData;
            materialData =
                std::make_unique<MaterialDataBase>(*properties.materialData);
        }
    }

    MaterialProperties &operator=(const MaterialProperties &properties)
    {
        if (this != &properties)
        {
            rasterData = properties.rasterData;
            raytraceData = properties.raytraceData;
            materialData =
                std::make_unique<MaterialDataBase>(*properties.materialData);
        }
        return *this;
    }

    MaterialProperties(MaterialProperties &&properties) noexcept
        : rasterData{std::move(properties.rasterData)},
          raytraceData{std::move(properties.raytraceData)},
          materialData{std::move(properties.materialData)}
    {
    }

    MaterialProperties &operator=(MaterialProperties &&properties) noexcept
    {
        rasterData = std::move(properties.rasterData);
        raytraceData = std::move(properties.raytraceData);
        materialData = std::move(properties.materialData);
        return *this;
    }
};

static const BasicShadedMaterialData DEFAULT_BASIC_SHADED_MATERIAL_DATA{
    math::Vector4{0.65f, 0.65f, 0.65f, 1.0f}};

static const OutlineMaterialData DEFAULT_OUTLINE_MATERIAL_DATA{
    math::Vector4{1.0f, 1.0f, 1.0f, 1.0f}, 0.01f};

static const WireframeMaterialData DEFAULT_WIREFRAME_MATERIAL_DATA{
    math::Vector4{0.0f, 0.0f, 0.0f, 0.0f}};

static const PrincipledMaterialData DEFAULT_PRINCIPLED_MATERIAL_DATA{
    math::Vector4{0.65f, 0.65f, 0.65f, 1.0f}};

} // namespace kirana::scene

#endif