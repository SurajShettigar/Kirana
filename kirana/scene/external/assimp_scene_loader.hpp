#ifndef KIRANA_SCENE_EXTERNAL_ASSIMP_SCENE_LOADER_HPP
#define KIRANA_SCENE_EXTERNAL_ASSIMP_SCENE_LOADER_HPP

#include "scene_loader.hpp"

namespace kirana::scene::external
{
class AssimpSceneLoader : public SceneLoader
{
  public:
    AssimpSceneLoader(const AssimpSceneLoader &sceneLoader) = delete;

    inline static AssimpSceneLoader &get()
    {
        static AssimpSceneLoader instance;
        return instance;
    }

    bool load(const std::string &path, const SceneImportSettings &settings,
              Scene *scene) override;
  private:
    AssimpSceneLoader() = default;
    ~AssimpSceneLoader() = default;

    static uint32_t getPostProcessMask(const SceneImportSettings &settings);
};
} // namespace kirana::scene::external
#endif