#include "plane.hpp"
#include "../mesh.hpp"
#include "../material.hpp"
#include "scene_utils.hpp"

kirana::scene::primitives::Plane::Plane(
    const std::shared_ptr<Material> &material)
    : Object{}
{
    m_name = "Plane";
    std::vector<Vertex> vertices{Vertex{math::Vector3{-0.5f, 0.0f, 0.5f},
                                        math::Vector3{0.0f, 1.0f, 0.0f},
                                        math::Vector3{0.0f, 0.0f, 0.0f}},
                                 Vertex{math::Vector3{0.5f, 0.0f, 0.5f},
                                        math::Vector3{0.0f, 1.0f, 0.0f},
                                        math::Vector3{0.0f, 0.0f, 0.0f}},
                                 Vertex{math::Vector3{0.5f, 0.0f, -0.5f},
                                        math::Vector3{0.0f, 1.0f, 0.0f},
                                        math::Vector3{0.0f, 0.0f, 0.0f}},
                                 Vertex{math::Vector3{-0.5f, 0.0f, -0.5f},
                                        math::Vector3{0.0f, 1.0f, 0.0f},
                                        math::Vector3{0.0f, 0.0f, 0.0f}}};
    std::vector<uint32_t> indices{0, 1, 3, 2, 3, 1};
    m_meshes.resize(1);
    m_meshes[0] = std::make_shared<Mesh>("Plane", vertices, indices, material);
}