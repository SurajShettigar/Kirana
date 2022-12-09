#include "viewport_scene.hpp"

#include "material.hpp"
#include "perspective_camera.hpp"
#include "primitives/plane.hpp"

#include <algorithm>

void kirana::scene::ViewportScene::addDefaultMaterials()
{
    for (const auto &m : Material::getDefaultMaterials())
        m_materials.emplace_back(std::make_shared<Material>(m));
}

void kirana::scene::ViewportScene::addDefaultRenderables()
{
    // TODO: Add renderables for camera, lights, gizmos, axis
    m_renderables.emplace_back(Renderable{m_grid.get(), true, false});
}

void kirana::scene::ViewportScene::onSceneLoaded()
{
    if (m_currentScene.isInitialized())
    {
        for (const auto &m : m_currentScene.getMaterials())
            m_materials.emplace_back(m);

        const auto &sceneObjects = m_currentScene.getObjects();
        for (size_t i = 0; i < sceneObjects.size(); i++)
        {
            const std::string &name = sceneObjects[i]->getName();
            m_sceneObjectIndexTable[name] = m_renderables.size();
            m_sceneObjectSelectionTable[name] = false;
            m_renderables.emplace_back(
                Renderable{sceneObjects[i].get(), false, false});
        }
        toggleObjectSelection(m_currentScene.getRoot()->getName());
    }
    m_onSceneLoaded(m_currentScene, m_currentScene.isInitialized());
}


void kirana::scene::ViewportScene::toggleObjectSelection(
    const std::string &objectName, bool multiSelect)
{
    // TODO: Find performant way to toggle selection
    if (!multiSelect)
    {
        for (auto &s : m_sceneObjectSelectionTable)
        {
            if (objectName != s.first)
            {
                m_renderables[m_sceneObjectIndexTable[s.first]].selected =
                    false;
                m_sceneObjectSelectionTable[s.first] = false;
            }
        }
    }
    if (objectName.empty())
        return;

    m_sceneObjectSelectionTable[objectName] =
        !m_sceneObjectSelectionTable[objectName];

    m_renderables[m_sceneObjectIndexTable[objectName]].selected =
        m_sceneObjectSelectionTable[objectName];
}

kirana::scene::ViewportScene::ViewportScene()
    : m_worldData{}, m_camera{std::make_unique<PerspectiveCamera>(
                         std::array<uint32_t, 2>{1280, 720}, 50.0f, 0.01f,
                         1000.0f, true, true)},
      m_grid{std::make_unique<primitives::Plane>(
          std::make_shared<Material>(Material::DEFAULT_MATERIAL_GRID))}
{
    addDefaultMaterials();
    addDefaultRenderables();
}

kirana::scene::ViewportScene::~ViewportScene()
{
}
