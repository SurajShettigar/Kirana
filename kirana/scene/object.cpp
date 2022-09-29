#include "object.hpp"
#include "mesh.hpp"
#include "scene_utils.hpp"

#include <assimp/scene.h>

kirana::scene::Object::Object(const aiNode *node,
                              const std::vector<std::shared_ptr<Mesh>> &meshes,
                              const std::shared_ptr<Object> &parent)
    : m_name{node->mName.C_Str()}, m_meshes{meshes}, m_parent{parent}
{

}