#ifndef KIRANA_SCENE_MATERIAL_HPP
#define KIRANA_SCENE_MATERIAL_HPP

#include "object.hpp"
#include "material_properties.hpp"

namespace kirana::scene
{
class Material : public Object, public MaterialProperties
{
    friend class external::AssimpSceneConverter;

  public:
    // Static Materials
    static const Material DEFAULT_MATERIAL_EDITOR_OUTLINE;
    static const Material DEFAULT_MATERIAL_EDITOR_GRID;

    static const Material DEFAULT_MATERIAL_BASIC_SHADED;
    static const Material DEFAULT_MATERIAL_WIREFRAME;
    static const Material DEFAULT_MATERIAL_SHADED;

    Material() = default;
    explicit Material(std::string name, std::string shaderName, bool isInternal,
                      RasterPipelineData rasterData,
                      RaytracePipelineData raytraceData,
                      const std::vector<MaterialParameter> &parameters)
        : Object(std::move(name)),
          MaterialProperties(std::move(rasterData), std::move(raytraceData),
                             parameters),
          m_shaderName{std::move(shaderName)}, m_isInternal{isInternal}
    {
        initShaderData();
    };
    ~Material() override = default;

    Material(const Material &mesh) = default;
    Material(Material &&mesh) = default;
    Material &operator=(const Material &mesh) = default;
    Material &operator=(Material &&mesh) = default;

  protected:
    std::string m_shaderName;
    bool m_isInternal;
    std::array<ShaderData,
               static_cast<int>(ShadingPipeline::SHADING_PIPELINE_MAX)>
        m_shaderData;

    [[nodiscard]] ShadingStage getShadingStage(
        const std::string &filePath) const;
    void initShaderData();
};

} // namespace kirana::scene

#endif