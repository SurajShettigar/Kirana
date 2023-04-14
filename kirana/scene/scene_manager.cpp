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
    scene.postProcess();
}

void kirana::scene::SceneManager::initEditorScene()
{
    std::vector<Material> materials;
    materials.push_back(Material::DEFAULT_MATERIAL_EDITOR_GRID);
    materials.push_back(Material::DEFAULT_MATERIAL_EDITOR_OUTLINE);
    materials.push_back(Material::DEFAULT_MATERIAL_BASIC_SHADED);
    materials.push_back(Material::DEFAULT_MATERIAL_WIREFRAME);
    materials.push_back(Material::DEFAULT_MATERIAL_SHADED);

    std::vector<Mesh> meshes;
    meshes.push_back(Mesh::DEFAULT_QUAD);

    std::vector<Camera> cameras;
    cameras.push_back(Camera::DEFAULT_PERSPECTIVE_CAM);

    m_editorScene = Scene{"EditorScene", meshes, materials, {}, cameras};
    // Set grid's mesh material to grid material
    m_editorScene.setMeshMaterial(0, 0);
    // Add root node
    const auto &rootNode = m_editorScene.addNode(
        -1, NodeObjectType::EMPTY, -1, math::Transform{}, "EditorScene");
    // Add grid mesh node
    const auto &gridNode = m_editorScene.addNode(
        rootNode.index, NodeObjectType::EMPTY, -1, math::Transform{}, "Grid");
    m_editorScene.addNode(gridNode.index, NodeObjectType::MESH, 0,
                          math::Transform{}, meshes[0].getName());
    // Add camera node
    const auto &camNode =
        m_editorScene.addNode(rootNode.index, NodeObjectType::CAMERA, 0,
                              math::Transform{}, cameras[0].getName());

    auto &camera = m_editorScene.getCameraAtNode(camNode);

    auto camTransform = m_editorScene.getGlobalTransform(camNode);

    camTransform.setPosition(math::Vector3{0.0f, 3.0f, 5.0f});
    camTransform.lookAt(math::Vector3::normalize(camTransform.getPosition()),
                        math::Vector3::UP);

    m_editorScene.setGlobalTransform(camNode, camTransform);
    postProcessScene("EditorScene", m_editorScene);
}

void kirana::scene::SceneManager::init()
{
    initEditorScene();
    m_sceneEditor.init();
}

void kirana::scene::SceneManager::update()
{
    m_sceneEditor.update();
}

void kirana::scene::SceneManager::clean()
{
    m_sceneEditor.clean();
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