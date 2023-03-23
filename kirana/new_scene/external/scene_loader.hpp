#ifndef KIRANA_SCENE_EXTERNAL_SCENE_LOADER_HPP
#define KIRANA_SCENE_EXTERNAL_SCENE_LOADER_HPP

#include <string>

namespace kirana::scene
{
struct SceneImportSettings;
class Scene;
} // namespace kirana::scene

namespace kirana::scene::external
{
class SceneLoader
{
  public:
    SceneLoader(const SceneLoader &) = delete;

    virtual bool load(const std::string &path,
                      const SceneImportSettings &settings, Scene *scene) = 0;

  protected:
    SceneLoader() = default;
    virtual ~SceneLoader() = default;
};
} // namespace kirana::scene::external
#endif