#ifndef SCENE_IMPORTER_HPP
#define SCENE_IMPORTER_HPP

#include "scene_types.hpp"
#include "scene.hpp"

namespace kirana::scene
{
class SceneImporter
{
  private:
    SceneImporter() = default;
    ~SceneImporter() = default;

    uint32_t getPostProcessMask(const SceneImportSettings &importSettings);

  public:
    SceneImporter(const SceneImporter &sceneImporter) = delete;

    static SceneImporter &get()
    {
        static SceneImporter instance;
        return instance;
    }

    bool loadSceneFromFile(const char *path,
                           const SceneImportSettings &importSettings, Scene *scene);
};
} // namespace kirana::scene
#endif