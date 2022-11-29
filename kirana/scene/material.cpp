#include "material.hpp"

#include <constants.h>

namespace constants = kirana::utils::constants;

kirana::scene::Material::Material(std::string name, std::string shader)
    : m_name{name.empty() ? constants::DEFAULT_SCENE_MATERIAL_NAME
                          : std::move(name)},
      m_shader{shader.empty() ? constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME
                              : std::move(shader)}
{
}


kirana::scene::Material::Material(const Material &material)
{
    if (this != &material)
    {
        m_name = material.m_name;
        m_shader = material.m_shader;
    }
}

kirana::scene::Material &kirana::scene::Material::operator=(
    const Material &material)
{
    if (this != &material)
    {
        m_name = material.m_name;
        m_shader = material.m_shader;
    }
    return *this;
}