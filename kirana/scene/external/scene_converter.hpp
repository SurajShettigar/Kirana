#ifndef KIRANA_SCENE_EXTERNAL_SCENE_CONVERTER_HPP
#define KIRANA_SCENE_EXTERNAL_SCENE_CONVERTER_HPP

namespace kirana::math
{
class Transform;
}

namespace kirana::scene
{
class Mesh;
class Image;
class Material;
class Light;
class Camera;
class Scene;
} // namespace kirana::scene

namespace kirana::scene::external
{
class SceneConverter
{
  public:
    SceneConverter(const SceneConverter &converter) = delete;


    virtual bool convertTransform(const void *inputTransform,
                                  math::Transform *outputTransform) = 0;
    virtual bool convertMesh(const void *inputMesh,
                             scene::Mesh *outputMesh) = 0;
    virtual bool convertTexture(const void *inputTexture,
                                scene::Image *outputTexture) = 0;
    virtual bool convertMaterial(const void *inputMaterial,
                                 scene::Material *outputMaterial) = 0;
    virtual bool convertLight(const void *inputLight,
                              scene::Light *outputLight) = 0;
    virtual bool convertCamera(const void *inputCamera,
                               scene::Camera *outputCamera) = 0;
    virtual bool convertScene(const void *inputScene,
                              scene::Scene *outputScene) = 0;

  protected:
    SceneConverter() = default;
    virtual ~SceneConverter() = default;
};
} // namespace kirana::scene::external
#endif