#include "viewport_scene.hpp"

#include "material.hpp"
#include "perspective_camera.hpp"
#include "primitives/plane.hpp"

#include <algorithm>

void kirana::scene::ViewportScene::initializeEditorObjects()
{
    m_editorMaterials.emplace_back(
        std::make_shared<Material>(Material::DEFAULT_MATERIAL_EDITOR_OUTLINE));
    m_editorMaterials.emplace_back(
        std::make_shared<Material>(Material::DEFAULT_MATERIAL_BASIC_SHADED));
    m_editorMaterials.emplace_back(
        std::make_shared<Material>(Material::DEFAULT_MATERIAL_WIREFRAME));
    m_editorMaterials.emplace_back(
        std::make_shared<Material>(Material::DEFAULT_MATERIAL_SHADED));

    m_grid = std::make_unique<primitives::Plane>(
        std::make_shared<Material>(Material::DEFAULT_MATERIAL_EDITOR_GRID));
    m_editorMaterials.emplace_back(m_grid->getMeshes()[0]->getMaterial());

    m_editorRenderables.emplace_back(
        Renderable{m_grid.get(), false, true, false, false});

    m_editorSceneInfo = SceneInfo{};
    m_editorSceneInfo.vertexSize = static_cast<uint32_t>(sizeof(scene::Vertex));
    m_editorSceneInfo.indexSize =
        static_cast<uint32_t>(sizeof(scene::INDEX_TYPE));
    for (const auto &m : m_grid->getMeshes())
    {
        m_editorSceneInfo.numVertices += m->getVertices().size();
        m_editorSceneInfo.numIndices += m->getIndices().size();
    }
    m_editorSceneInfo.totalVertexSize =
        static_cast<size_t>(m_editorSceneInfo.vertexSize) *
        static_cast<size_t>(m_editorSceneInfo.numVertices);
    m_editorSceneInfo.totalIndexSize =
        static_cast<size_t>(m_editorSceneInfo.indexSize) *
        static_cast<size_t>(m_editorSceneInfo.numIndices);

    // TODO: Add renderables for camera, lights, gizmos, axis
}

void kirana::scene::ViewportScene::onSceneLoaded()
{
    if (m_currentScene.isInitialized())
    {

        const auto &sceneObjects = m_currentScene.getObjects();
        for (uint32_t i = 0; i < sceneObjects.size(); i++)
        {
            const std::string &name = sceneObjects[i]->getName();
            m_objectIndexTable[name] = m_sceneRenderables.size();
            m_objectSelectionTable[name] = false;
            m_sceneRenderables.emplace_back(
                Renderable{sceneObjects[i].get(), true, true, true, false});
        }
        m_sceneInfo = SceneInfo{};
        m_sceneInfo.vertexSize = static_cast<uint32_t>(sizeof(scene::Vertex));
        m_sceneInfo.indexSize =
            static_cast<uint32_t>(sizeof(scene::INDEX_TYPE));
        for (const auto &m : m_currentScene.getMeshes())
        {
            m_sceneInfo.numVertices += m->getVertices().size();
            m_sceneInfo.numIndices += m->getIndices().size();
        }
        m_sceneInfo.totalVertexSize =
            static_cast<size_t>(m_sceneInfo.vertexSize) *
            static_cast<size_t>(m_sceneInfo.numVertices);
        m_sceneInfo.totalIndexSize =
            static_cast<size_t>(m_sceneInfo.indexSize) *
            static_cast<size_t>(m_sceneInfo.numIndices);

        toggleObjectSelection(m_currentScene.getRoot()->getName());
    }
    m_isSceneLoaded = true;
    m_onSceneLoaded(m_currentScene.isInitialized());
}


void kirana::scene::ViewportScene::toggleObjectSelection(
    const std::string &objectName, bool multiSelect)
{
    // TODO: Find performant way to toggle selection
    if (!multiSelect)
    {
        for (auto &s : m_objectSelectionTable)
        {
            if (objectName != s.first)
            {
                m_sceneRenderables[m_objectIndexTable[s.first]].selected =
                    false;
                m_objectSelectionTable[s.first] = false;
            }
        }
    }
    if (objectName.empty())
        return;

    auto &r = m_sceneRenderables[m_objectIndexTable[objectName]];
    if (r.selectable)
    {
        m_objectSelectionTable[objectName] =
            !m_objectSelectionTable[objectName];
        r.selected = m_objectSelectionTable[objectName];
    }
}

kirana::scene::ViewportScene::ViewportScene()
    : m_worldData{}, m_camera{std::make_unique<PerspectiveCamera>(
                         std::array<uint32_t, 2>{1280, 720}, 50.0f, 0.01f,
                         1000.0f, true, true)},
      m_isSceneLoaded{false}
{
    initializeEditorObjects();
}

kirana::scene::ViewportScene::~ViewportScene()
{
}
