#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <vector>
#include <array>
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

    Material();
    explicit Material(std::string shaderName, std::string materialName,
                      MaterialProperties properties,
                      bool isEditorMaterial = false);
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
    [[nodiscard]] inline const ShaderData &getShaderData(
        ShadingPipeline currentPipeline) const
    {
        return m_isEditorMaterial
                   ? m_shaderData[0]
                   : m_shaderData[static_cast<int>(currentPipeline)];
    }
    [[nodiscard]] inline const RasterPipelineData &getRasterPipelineData() const
    {
        return m_properties.rasterData;
    }
    [[nodiscard]] inline const RaytracePipelineData &getRaytracePipelineData()
        const
    {
        return m_properties.raytraceData;
    }

    inline void setName(const std::string &name)
    {
        m_name = name;
    }

    [[nodiscard]] inline const MaterialParameter &getMaterialParameter(
        const std::string &parameter) const
    {
        return *std::find_if(m_properties.parameters.begin(),
                             m_properties.parameters.end(),
                             [&parameter](const MaterialParameter &p) {
                                 return parameter == p.id;
                             });
    }

    inline void getMaterialParameterData(std::vector<uint8_t> *dataBuffer) const
    {
        m_properties.getParametersData(dataBuffer);
    }

    bool setMaterialParameter(const std::string &parameter,
                              const std::any &value)
    {
        auto it = std::find_if(m_properties.parameters.begin(),
                               m_properties.parameters.end(),
                               [&parameter](const MaterialParameter &p) {
                                   return parameter == p.id;
                               });
        // TODO: Check type of parameter value.
        if (it == m_properties.parameters.end())
            return false;
        it->value = value;
        return true;
    }

  private:
    std::string m_shaderName = "";
    std::string m_name = "";
    MaterialProperties m_properties;
    bool m_isEditorMaterial = false;
    std::array<ShaderData,
               static_cast<int>(ShadingPipeline::SHADING_PIPELINE_MAX)>
        m_shaderData;

    void setShaderData();
    static std::string getMaterialNameFromShaderName(
        const std::string &shaderName);
};
} // namespace kirana::scene

#endif