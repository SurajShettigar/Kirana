#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <glm/vec3.hpp>

namespace kirana::scene
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};
} // namespace kirana::scene

#endif