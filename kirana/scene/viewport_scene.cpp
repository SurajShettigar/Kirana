#include "viewport_scene.hpp"

#include "material.hpp"
#include "perspective_camera.hpp"
#include "primitives/plane.hpp"


void kirana::scene::ViewportScene::onSceneLoaded()
{
    m_selectedObjects.clear();
    if (m_currentScene.isInitialized())
    {
        m_selectedObjects.push_back(0);
        const auto &sceneObjects = m_currentScene.getObjects();
        for (size_t i = 0; i < sceneObjects.size(); i++)
        {
            // TODO: Add support for selected objects (Outline material).
            m_renderables.emplace_back(
                Renderable{sceneObjects[i].get(), false, nullptr, false});
        }
    }
    m_onSceneLoaded(m_currentScene, m_currentScene.isInitialized());
}

kirana::scene::ViewportScene::ViewportScene()
    : m_worldData{}, m_camera{std::make_unique<PerspectiveCamera>(
                         std::array<uint32_t, 2>{1280, 720}, 50.0f, 0.01f,
                         1000.0f, true, true)},
      m_grid{std::make_unique<primitives::Plane>(
          std::make_shared<Material>(Material::DEFAULT_MATERIAL_GRID))}
{
    m_renderables.emplace_back(Renderable{m_grid.get(), true});
}

kirana::scene::ViewportScene::~ViewportScene()
{
}
