#ifndef KIRANA_SCENE_CONVERTERS_ASSIMP_CONVERTER_HPP
#define KIRANA_SCENE_CONVERTERS_ASSIMP_CONVERTER_HPP

#include "converter.hpp"
#include <string>

struct aiScene;
struct aiNode;

namespace kirana::scene::converters
{
class AssimpConverter : public Converter
{
  public:
    AssimpConverter(const AssimpConverter &converter) = delete;

    static AssimpConverter &get()
    {
        static AssimpConverter instance;
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
    struct AssimpTexture {
        std::string filePath;

    };
    AssimpConverter() = default;
    ~AssimpConverter() override = default;

    bool generateSceneGraph(
        const aiScene &scene, scene::Scene *outputScene, const aiNode &node,
        int parent);
};

} // namespace kirana::scene::converters

#endif // KIRANA_SCENE_CONVERTERS_ASSIMP_CONVERTER_HPP
