#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <glm/vec3.hpp>
#include <glm/common.hpp>

namespace kirana::scene
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct SceneImportSettings
{
    bool calculateTangentSpace = false;
    bool joinIdenticalVertices = false;
    bool triangulate = true;
    bool generateNormals = false;
    bool generateSmoothNormals = false;
    bool improveCacheLocality = true;
};

static const SceneImportSettings DEFAULT_SCENE_IMPORT_SETTINGS{};

} // namespace kirana::scene

#endif