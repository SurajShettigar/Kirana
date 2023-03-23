#include "scene_manager.hpp"

#include "external/assimp_scene_loader.hpp"

#include <constants.h>
#include <file_system.hpp>

using kirana::utils::filesystem::combinePath;
using kirana::utils::filesystem::fileExists;
using kirana::utils::filesystem::getFilename;
namespace constants = kirana::utils::constants;


void kirana::scene::SceneManager::postProcessScene(const std::string &scenePath,
                                                   Scene &scene)
{
    if (scene.getName().empty())
    {
        auto fName = getFilename(scenePath);
        scene.setName(fName.first + fName.second);
    }
}

bool kirana::scene::SceneManager::loadScene(const std::string &scenePath,
                                            const SceneImportSettings &settings)
{
    if (!fileExists(scenePath))
        return false;
    if (!external::AssimpSceneLoader::get().load(scenePath, settings,
                                                 &m_currentScene))
        return false;
    postProcessScene(scenePath, m_currentScene);
    return true;
}


bool kirana::scene::SceneManager::loadDefaultScene()
{
    return loadScene(
        combinePath(constants::DATA_DIR_PATH, {constants::DEFAULT_MODEL_NAME}));
}