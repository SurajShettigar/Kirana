#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <vector>

namespace kirana::scene
{
class Material
{
  public:
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
    static const StencilProperties STENCIL_PROPERTIES_WRITE;
    static const StencilProperties STENCIL_PROPERTIES_READ;

    struct MaterialProperties
    {
        bool renderWireframe = false;
        float wireframeWidth = 1.0f;
        CullMode cullMode = CullMode::BACK;
        SurfaceType surfaceType = SurfaceType::OPAQUE;
        bool enableDepth = true;
        bool writeDepth = true;
        CompareOperation depthCompareOp = CompareOperation::LESS_OR_EQUAL;
        StencilProperties stencil;
    };

    // Static Materials
    static const Material DEFAULT_MATERIAL_MAT_CAP;
    static const Material DEFAULT_MATERIAL_WIREFRAME;
    static const Material DEFAULT_MATERIAL_GRID;
    static const Material DEFAULT_MATERIAL_CAMERA;
    static const Material DEFAULT_MATERIAL_LIGHT;
    static const Material DEFAULT_MATERIAL_OUTLINE;
    static const Material DEFAULT_MATERIAL_SCENE;

    // Events
    // TODO: Add Material change event

    Material() = default;
    explicit Material(const std::string &name, const std::string &shader,
                      const MaterialProperties &properties);
    ~Material() = default;

    Material(const Material &material);
    Material &operator=(const Material &material);

    // Getters-Setters
    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::string &getShader() const
    {
        return m_shader;
    }
    [[nodiscard]] inline const MaterialProperties &getProperties() const
    {
        return m_properties;
    }
    // TODO: Add setters and call material change event
    inline void setProperties(const MaterialProperties &properties)
    {
        m_properties = properties;
    }

    // Static functions
    inline static std::vector<Material> getDefaultMaterials()
    {
        return {DEFAULT_MATERIAL_MAT_CAP, DEFAULT_MATERIAL_WIREFRAME,
                DEFAULT_MATERIAL_GRID,    DEFAULT_MATERIAL_CAMERA,
                DEFAULT_MATERIAL_LIGHT,   DEFAULT_MATERIAL_OUTLINE,
                DEFAULT_MATERIAL_SCENE};
    }

  private:
    std::string m_name = "Material";
    std::string m_shader = "MatCap";
    MaterialProperties m_properties;
};
} // namespace kirana::scene

#endif