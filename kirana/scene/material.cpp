#include "material.hpp"

#include <constants.h>

namespace constants = kirana::utils::constants;

kirana::scene::Material::Material(std::string name, std::string shader)
    : m_name{name.empty() ? constants::DEFAULT_SCENE_MATERIAL_NAME : name},
      m_shader{shader.empty() ? constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME
                              : shader}
{
}