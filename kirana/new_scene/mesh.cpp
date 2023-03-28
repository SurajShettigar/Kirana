#include "mesh.hpp"

const kirana::scene::Mesh kirana::scene::Mesh::DEFAULT_QUAD{
    "Quad",
    std::vector<Vertex>{
        Vertex{
            math::Vector3{-0.5f, 0.0f, 0.5f}, math::Vector3{0.0f, 1.0f, 0.0f},
            math::Vector4{0.0f, 0.0f, 0.0f, 1.0f}, math::Vector2{0.0f, 0.0f}},
        Vertex{math::Vector3{0.5f, 0.0f, 0.5f}, math::Vector3{0.0f, 1.0f, 0.0f},
               math::Vector4{0.0f, 0.0f, 0.0f, 1.0f},
               math::Vector2{1.0f, 0.0f}},
        Vertex{
            math::Vector3{0.5f, 0.0f, -0.5f}, math::Vector3{0.0f, 1.0f, 0.0f},
            math::Vector4{0.0f, 0.0f, 0.0f, 1.0f}, math::Vector2{1.0f, 1.0f}},
        Vertex{
            math::Vector3{-0.5f, 0.0f, -0.5f}, math::Vector3{0.0f, 1.0f, 0.0f},
            math::Vector4{0.0f, 0.0f, 0.0f, 1.0f}, math::Vector2{0.0f, 1.0f}}},
    std::vector<INDEX_TYPE>{0, 1, 3, 2, 3, 1},
    math::Bounds3{math::Vector3{-0.5f, -0.01f, -0.05f},
                  math::Vector3{0.5f, 0.01f, 0.05f}}};