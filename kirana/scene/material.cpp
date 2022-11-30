#include "material.hpp"

#include <constants.h>

namespace constants = kirana::utils::constants;

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_MAT_CAP{
    constants::DEFAULT_MATERIAL_MAT_CAP_NAME,
    constants::VULKAN_SHADER_MAT_CAP_NAME, MaterialProperties{}};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        constants::DEFAULT_MATERIAL_WIREFRAME_NAME,
        constants::VULKAN_SHADER_WIREFRAME_NAME, MaterialProperties{true, 1.0}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_GRID{
    constants::DEFAULT_MATERIAL_GRID_NAME, constants::VULKAN_SHADER_GRID_NAME,
    MaterialProperties{false, 1.0, CullMode::None, SurfaceType::Transparent}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_CAMERA{
    constants::DEFAULT_MATERIAL_CAMERA_NAME,
    constants::VULKAN_SHADER_WIREFRAME_NAME, MaterialProperties{true, 1.0}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_LIGHT{
    constants::DEFAULT_MATERIAL_LIGHT_NAME,
    constants::VULKAN_SHADER_WIREFRAME_NAME, MaterialProperties{true, 1.0}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_SCENE{
    constants::DEFAULT_SCENE_MATERIAL_NAME,
    constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME, MaterialProperties{}};

kirana::scene::Material::Material(const std::string &name,
                                  const std::string &shader,
                                  const MaterialProperties &properties)
    : m_name{name.empty() ? constants::DEFAULT_SCENE_MATERIAL_NAME : name},
      m_shader{shader.empty() ? constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME
                              : shader},
      m_properties{properties}
{
}


kirana::scene::Material::Material(const Material &material)
{
    if (this != &material)
    {
        m_name = material.m_name;
        m_shader = material.m_shader;
        m_properties = material.m_properties;
    }
}

kirana::scene::Material &kirana::scene::Material::operator=(
    const Material &material)
{
    if (this != &material)
    {
        m_name = material.m_name;
        m_shader = material.m_shader;
        m_properties = material.m_properties;
    }
    return *this;
}