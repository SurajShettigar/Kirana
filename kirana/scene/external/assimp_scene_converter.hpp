#ifndef KIRANA_SCENE_EXTERNAL_ASSIMP_SCENE_CONVERTER_HPP
#define KIRANA_SCENE_EXTERNAL_ASSIMP_SCENE_CONVERTER_HPP

#include "scene_converter.hpp"
#include <string>
#include <unordered_map>

struct aiScene;
struct aiNode;

namespace kirana::scene::external
{
class AssimpSceneConverter : public SceneConverter
{
  public:
    AssimpSceneConverter(const AssimpSceneConverter &converter) = delete;

    static AssimpSceneConverter &get()
    {
        static AssimpSceneConverter instance;
        return instance;
    }

    bool convertTransform(const void *inputTransform,
                          math::Transform *outputTransform) override;
    bool convertMesh(const void *inputMesh, scene::Mesh *outputMesh) override;
    bool convertTexture(const void *inputTexture,
                        scene::Image *outputTexture) override;
    bool convertMaterial(const void *inputMaterial,
                         scene::Material *outputMaterial) override;
    bool convertLight(const void *inputLight,
                      scene::Light *outputLight) override;
    bool convertCamera(const void *inputCamera,
                       scene::Camera *outputCamera) override;
    bool convertScene(const void *inputScene,
                      scene::Scene *outputScene) override;

  private:
    std::unordered_map<std::string, uint32_t> m_lightNameIndexTable;
    std::unordered_map<std::string, uint32_t> m_cameraNameIndexTable;
    AssimpSceneConverter() = default;
    ~AssimpSceneConverter() = default;

    bool generateSceneGraph(const aiScene &scene, scene::Scene *outputScene,
                            const aiNode &node, int parent);
};

} // namespace kirana::scene::external

#endif
