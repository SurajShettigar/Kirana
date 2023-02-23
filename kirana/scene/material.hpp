#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <vector>
#include <array>
#include <event.hpp>

#include "material_properties.hpp"

struct aiMaterial;

namespace kirana::scene
{
class Material : public MaterialProperties
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

    Material();
    explicit Material(const aiMaterial *material);
    explicit Material(std::string shaderName, std::string materialName,
                      const RasterPipelineData &rasterData,
                      const RaytracePipelineData &raytraceData,
                      const std::vector<MaterialParameter> &parameters,
                      bool isEditorMaterial = false);
    ~Material() = default;
    Material(const Material &material) = default;
    Material &operator=(const Material &material) = default;
    Material(Material &&material) = default;
    Material &operator=(Material &&material) = default;

    // Getters-Setters
    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::string &getShaderName() const
    {
        return m_shaderName;
    }
    [[nodiscard]] inline const ShaderData &getShaderData(
        ShadingPipeline currentPipeline) const
    {
        return m_isEditorMaterial
                   ? m_shaderData[0]
                   : m_shaderData.at(static_cast<int>(currentPipeline));
    }

    inline void setName(const std::string &name)
    {
        m_name = name;
    }

  private:
    std::string m_shaderName;
    std::string m_name;
    bool m_isEditorMaterial = false;
    std::array<ShaderData,
               static_cast<int>(ShadingPipeline::SHADING_PIPELINE_MAX)>
        m_shaderData;

    [[nodiscard]] ShadingStage getShadingStage(
        const std::string &filePath) const;
    void setShaderData();

    static std::string getMaterialNameFromShaderName(
        const std::string &shaderName);

    void setParametersFromAiMaterial(const aiMaterial *material);
};
} // namespace kirana::scene

#endif