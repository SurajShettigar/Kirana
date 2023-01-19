#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <vector>
#include "material_types.hpp"

namespace kirana::scene
{
class Material
{
  public:
    // Static Materials
    static const Material DEFAULT_MATERIAL_EDITOR_OUTLINE;
    static const Material DEFAULT_MATERIAL_EDITOR_GRID;

    static const Material DEFAULT_MATERIAL_BASIC_SHADED;
    static const Material DEFAULT_MATERIAL_WIREFRAME;
    static const Material DEFAULT_MATERIAL_SHADED;

    // Static functions
    inline static std::vector<Material> getEditorMaterials()
    {
        return {DEFAULT_MATERIAL_EDITOR_OUTLINE, DEFAULT_MATERIAL_EDITOR_GRID};
    }
    inline static std::vector<Material> getDefaultMaterials()
    {
        return {DEFAULT_MATERIAL_BASIC_SHADED, DEFAULT_MATERIAL_WIREFRAME,
                DEFAULT_MATERIAL_SHADED};
    }

    // Events
    // TODO: Add Material change event

    Material() = default;
    explicit Material(
        const std::string &name, const std::string &shader,
        const MaterialProperties &properties,
        ShadingPipeline currentTechnique = ShadingPipeline::RASTER);
    ~Material() = default;

    Material(const Material &material);
    Material &operator=(const Material &material);

    // Getters-Setters
    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::string &getShaderName() const
    {
        return m_shaderName;
    }
    [[nodiscard]] inline const std::string &getShaderPath() const
    {
        return m_shaderPath;
    }
    [[nodiscard]] inline const MaterialDataBase *getMaterialData() const
    {
        return m_properties.materialData.get();
    }
    inline const MaterialDataBase *getCurrentPipelineData() const
    {
        return m_currentPipeline == ShadingPipeline::RASTER
                   ? &m_properties.rasterData
                   : &m_properties.raytraceData;
    }

    inline void setMaterialData(const MaterialDataBase *data)
    {
        *m_properties.materialData = *data;
    }

    inline void setPipeline(ShadingPipeline pipeline)
    {
        m_currentPipeline = pipeline;
        setShaderPath();
    }

  private:
    std::string m_name = "";
    std::string m_shaderName = "";
    std::string m_shaderPath = "";
    ShadingPipeline m_currentPipeline = ShadingPipeline::RASTER;
    MaterialProperties m_properties;

    void setShaderPath();
};
} // namespace kirana::scene

#endif