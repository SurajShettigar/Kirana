#ifndef SCENE_HPP
#define SCENE_HPP

#include "object.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "camera.hpp"

#include <memory>
#include <vector>
#include <string>

struct aiScene;
struct aiNode;

namespace kirana::scene
{
class SceneImporter;
class SceneManager;
class Scene
{
    friend class SceneImporter;
    friend class SceneManager;

  private:
    bool m_isInitialized = false;

    // Scene data
    std::string m_name = "Scene";
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Object>>
        m_objects; // Also contains root object.
    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<Camera> m_cameras;

    void getMeshesFromNode(const aiNode *node,
                           std::vector<std::shared_ptr<Mesh>> *nodeMeshes,
                           math::Bounds3 *bounds);
    void initializeChildObjects(std::shared_ptr<Object> parent,
                                uint32_t childCount, aiNode **children);
    void initFromAiScene(const aiScene *scene);

  public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene &scene) = delete;

    // Getters-Setters
    [[nodiscard]] inline bool isInitialized() const
    {
        return m_isInitialized;
    }
    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
    }
    [[nodiscard]] inline const std::shared_ptr<Object> &getRoot() const
    {
        return m_objects[0];
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Object>>
        &getObjects() const
    {
        return m_objects;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Material>>
        &getMaterials() const
    {
        return m_materials;
    }
    [[nodiscard]] inline const Camera &getActiveCamera() const
    {
        // TODO: Replace with current active camera.
        return m_cameras[0];
    }

    // Helper-Functions
    [[nodiscard]] std::vector<math::Transform *> getTransformsForMesh(
        const Mesh *mesh) const;
};
} // namespace kirana::scene
#endif