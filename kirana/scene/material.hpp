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
        None = 0,
        Front = 1,
        Back = 2,
        Both = 3
    };
    enum class SurfaceType
    {
        Opaque = 0,
        Transparent = 1,
    };
    struct MaterialProperties
    {
        bool renderWireframe = false;
        float wireframeWidth = 1.0f;
        CullMode cullMode = CullMode::Back;
        SurfaceType surfaceType = SurfaceType::Opaque;
    };

    // Static Materials
    static const Material DEFAULT_MATERIAL_MAT_CAP;
    static const Material DEFAULT_MATERIAL_WIREFRAME;
    static const Material DEFAULT_MATERIAL_GRID;
    static const Material DEFAULT_MATERIAL_CAMERA;
    static const Material DEFAULT_MATERIAL_LIGHT;
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


    // Static functions
    inline static std::vector<Material> getDefaultMaterials()
    {
        return {DEFAULT_MATERIAL_MAT_CAP, DEFAULT_MATERIAL_WIREFRAME,
                DEFAULT_MATERIAL_GRID,    DEFAULT_MATERIAL_CAMERA,
                DEFAULT_MATERIAL_LIGHT,   DEFAULT_MATERIAL_SCENE};
    }

  private:
    std::string m_name = "Material";
    std::string m_shader = "MatCap";
    MaterialProperties m_properties;
};
} // namespace kirana::scene

#endif