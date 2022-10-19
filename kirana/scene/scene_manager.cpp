#include "scene_manager.hpp"

#include "scene_importer.hpp"
#include <file_system.hpp>
#include <constants.h>

namespace constants = kirana::utils::constants;

const kirana::scene::Scene &kirana::scene::SceneManager::loadScene(
    std::string path, const SceneImportSettings &importSettings)
{
    if (path.empty())
        path = utils::filesystem::combinePath(constants::DATA_DIR_PATH,
                                              {constants::DEFAULT_MODEL_NAME});

    scene::SceneImporter::get().loadSceneFromFile(path.c_str(), importSettings,
                                                  &m_currentScene);
    return m_currentScene;
}

void kirana::scene::SceneManager::init()
{
    if (m_currentScene.isInitialized())
    {
//        m_currentScene.getCamera().transform.translate(
//            kirana::math::Vector3(0.0f, 1.5f, 3.0f));
        m_currentScene.getCamera().transform.translate(
            kirana::math::Vector3(0.0f, 8.0f, 10.0f));
        m_currentScene.getCamera().transform.lookAt(kirana::math::Vector3::ZERO,
                                                    kirana::math::Vector3::UP);
    }
}

void kirana::scene::SceneManager::update()
{
    if (m_currentScene.isInitialized())
    {
       /* m_currentScene.m_worldData.sunColor[0] = (float)std::max(std::sin(m_frameNum / 1000.0), 0.1);
        m_currentScene.m_worldData.sunColor[1] = (float)std::max(std::sin(m_frameNum / 1000.0), 0.1);
        m_currentScene.m_worldData.sunColor[2] = (float)std::max(std::sin(m_frameNum / 1000.0), 0.1);*/
        m_currentScene.getRoot()->transform->rotateY(1.0f);
    }
    m_frameNum++;
}

void kirana::scene::SceneManager::clean()
{
}