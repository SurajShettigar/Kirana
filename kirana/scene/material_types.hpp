#ifndef MATERIAL_TYPES_HPP
#define MATERIAL_TYPES_HPP

#include <array>
#include <unordered_map>
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

struct WireframeMaterialData : MaterialDataBase
{
    math::Vector4 color = math::Vector4::ONE;

    explicit WireframeMaterialData(
        const math::Vector4 &color = math::Vector4::ONE)
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

struct MaterialProperties
{
    RasterPipelineData rasterData{};
    RaytracePipelineData raytraceData{};
    std::unordered_map<std::string, MaterialParameter> parameters;

    MaterialProperties() = default;

    MaterialProperties(
        RasterPipelineData rasterData, RaytracePipelineData raytraceData,
        const std::unordered_map<std::string, MaterialParameter> &parameters)
        : rasterData{std::move(rasterData)},
          raytraceData{std::move(raytraceData)}, parameters{parameters}
    {
    }

    ~MaterialProperties() = default;

    MaterialProperties(const MaterialProperties &properties)
    {
        if (this != &properties)
        {
            rasterData = properties.rasterData;
            raytraceData = properties.raytraceData;
            parameters = properties.parameters;
        }
    }

    MaterialProperties &operator=(const MaterialProperties &properties)
    {
        if (this != &properties)
        {
            rasterData = properties.rasterData;
            raytraceData = properties.raytraceData;
            parameters = properties.parameters;
        }
        return *this;
    }

    MaterialProperties(MaterialProperties &&properties) noexcept
        : rasterData{std::move(properties.rasterData)},
          raytraceData{std::move(properties.raytraceData)},
          parameters{std::move(properties.parameters)}
    {
    }

    MaterialProperties &operator=(MaterialProperties &&properties) noexcept
    {
        rasterData = std::move(properties.rasterData);
        raytraceData = std::move(properties.raytraceData);
        parameters = std::move(properties.parameters);
        return *this;
    }

    void getParametersData(std::vector<uint8_t> *dataBuffer) const
    {
        const auto &align = [](size_t size, size_t alignment) {
            return alignment > 0 ? (size + alignment - 1) & ~(alignment - 1)
                                 : size;
        };

        dataBuffer->resize(sizeof(uint8_t));

        size_t bufferSize = 0;
        for (const auto &p : parameters)
        {
            const int type = static_cast<int>(p.second.type);

            size_t alignment = 4;
            if (type > 6 && type < 10)
                alignment = 8;
            else if (type == 10)
                alignment = sizeof(math::Vector2);
            else if (type > 11)
                alignment = sizeof(math::Vector4);


            size_t bufferOffset = align(bufferSize, alignment);
            bufferSize = bufferOffset + alignment;
            dataBuffer->resize(bufferSize);

            void *bufferPtr =
                reinterpret_cast<void *>(dataBuffer->data() + bufferOffset);
            switch (p.second.type)
            {
            case MaterialParameterType::BOOL:
                memcpy(bufferPtr, std::any_cast<bool>(&p.second.value),
                       sizeof(bool));
                break;
            case MaterialParameterType::INT:
                memcpy(bufferPtr, std::any_cast<int>(&p.second.value),
                       sizeof(int));
                break;
            case MaterialParameterType::UINT:
                memcpy(bufferPtr, std::any_cast<uint32_t>(&p.second.value),
                       sizeof(uint32_t));
                break;
            case MaterialParameterType::FLOAT:
                memcpy(bufferPtr, std::any_cast<float>(&p.second.value),
                       sizeof(float));
                break;
            case MaterialParameterType::TEX_1D:
            case MaterialParameterType::TEX_2D:
            case MaterialParameterType::TEX_3D:
                memcpy(bufferPtr, std::any_cast<uint32_t>(&p.second.value),
                       sizeof(uint32_t));
                break;
            case MaterialParameterType::INT64:
                memcpy(bufferPtr, std::any_cast<int64_t>(&p.second.value),
                       sizeof(int64_t));
                break;
            case MaterialParameterType::UINT64:
                memcpy(bufferPtr, std::any_cast<uint64_t>(&p.second.value),
                       sizeof(uint64_t));
                break;
            case MaterialParameterType::DOUBLE:
                memcpy(bufferPtr, std::any_cast<double>(&p.second.value),
                       sizeof(double));
                break;
            case MaterialParameterType::VEC_2:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector2>(&p.second.value)->data(),
                       sizeof(math::Vector2));
                break;
            case MaterialParameterType::VEC_3:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector3>(&p.second.value)->data(),
                       sizeof(math::Vector3));
                break;
            case MaterialParameterType::VEC_4:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector4>(&p.second.value)->data(),
                       sizeof(math::Vector4));
                break;
            case MaterialParameterType::MAT_2x2:
                // TODO: Matrix 2x2 not yet implemented
                memcpy(bufferPtr,
                       std::any_cast<std::array<std::array<float, 2>, 2>>(
                           &p.second.value)
                           ->data(),
                       sizeof(float) * 4);
                break;
            case MaterialParameterType::MAT_3x3:
                // TODO: Matrix 3x3 not yet implemented
                memcpy(bufferPtr,
                       std::any_cast<std::array<std::array<float, 3>, 3>>(
                           &p.second.value)
                           ->data(),
                       sizeof(float) * 9);
                break;
            case MaterialParameterType::MAT_3x4:
                // TODO: Matrix 3x4 not yet implemented
                memcpy(bufferPtr,
                       std::any_cast<std::array<std::array<float, 3>, 4>>(
                           &p.second.value)
                           ->data(),
                       sizeof(float) * 12);
                break;
            case MaterialParameterType::MAT_4x4:
                // TODO: Matrix 4x4 implement data pointer
                std::array<std::array<float, 4>, 4> mat;
                const auto *m4x4 =
                    std::any_cast<math::Matrix4x4>(&p.second.value);
                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        mat[i][j] = (*m4x4)[i][j];
                memcpy(bufferPtr, mat.data(), mat.size());
                break;
            }
        }
        bufferSize = align(bufferSize, 16);
        dataBuffer->resize(bufferSize);
    }
};

static const std::unordered_map<std::string, MaterialParameter>
    DEFAULT_BASIC_SHADED_MATERIAL_PARAMETERS{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.65f, 0.65f, 0.65f, 1.0f)}}};

static const std::unordered_map<std::string, MaterialParameter>
    DEFAULT_OUTLINE_MATERIAL_PARAMETERS{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(1.0f, 1.0f, 0.0f, 1.0f)}},
        {"_Thickness",
         MaterialParameter{"_Thickness", MaterialParameterType::FLOAT, 0.02f}}};

static const std::unordered_map<std::string, MaterialParameter>
    DEFAULT_WIREFRAME_MATERIAL_PARAMETERS{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.0f, 0.0f, 0.0f, 1.0f)}}};

static const std::unordered_map<std::string, MaterialParameter>
    DEFAULT_BASIC_SHADED_WIREFRAME_MATERIAL_PARAMETERS{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.65f, 0.65f, 0.65f, 1.0f)}},
        {"_WireframeColor",
         MaterialParameter{"_WireframeColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.0f, 0.0f, 0.0f, 1.0f)}}};

static const std::unordered_map<std::string, MaterialParameter>
    DEFAULT_PRINCIPLED_MATERIAL_PARAMETERS{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           math::Vector4(0.85f, 0.85f, 0.85f, 1.0f)}},
        {"_SubSurface",
         MaterialParameter{"_SubSurface", MaterialParameterType::FLOAT, 0.0f}},
        {"_Metallic",
         MaterialParameter{"_Metallic", MaterialParameterType::FLOAT, 0.0f}},
        {"_Specular",
         MaterialParameter{"_Specular", MaterialParameterType::FLOAT, 0.0f}},
        {"_SpecularTint",
         MaterialParameter{"_SpecularTint", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Roughness",
         MaterialParameter{"_Roughness", MaterialParameterType::FLOAT, 0.5f}},
        {"_Anisotropic",
         MaterialParameter{"_Anisotropic", MaterialParameterType::FLOAT, 0.0f}},
        {"_Sheen",
         MaterialParameter{"_Sheen", MaterialParameterType::FLOAT, 0.0f}},
        {"_SheenTint",
         MaterialParameter{"_SheenTint", MaterialParameterType::FLOAT, 0.0f}},
        {"_ClearCoat",
         MaterialParameter{"_ClearCoat", MaterialParameterType::FLOAT, 0.0f}},
        {"_ClearCoatGloss",
         MaterialParameter{"_ClearCoatGloss", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Transmission",
         MaterialParameter{"_Transmission", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Ior",
         MaterialParameter{"_Ior", MaterialParameterType::FLOAT, 1.0f}}};

} // namespace kirana::scene

#endif