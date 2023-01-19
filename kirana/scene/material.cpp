#include "material.hpp"

#include <constants.h>

namespace constants = kirana::utils::constants;

const kirana::scene::Material::StencilProperties
    kirana::scene::Material::STENCIL_PROPERTIES_WRITE{true};
const kirana::scene::Material::StencilProperties
    kirana::scene::Material::STENCIL_PROPERTIES_READ{
        true,
        CompareOperation::NOT_EQUAL,
        StencilOperation::KEEP,
        StencilOperation::KEEP,
        StencilOperation::REPLACE,
        1};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_BASIC_SHADED{
    constants::DEFAULT_MATERIAL_BASIC_SHADED_NAME,
    constants::VULKAN_SHADER_BASIC_SHADED_NAME,
    MaterialProperties{false, 1.0f, CullMode::BACK, SurfaceType::OPAQUE, true,
                       true, CompareOperation::LESS_OR_EQUAL,
                       STENCIL_PROPERTIES_WRITE}};

const kirana::scene::Material
    kirana::scene::Material::DEFAULT_MATERIAL_WIREFRAME{
        constants::DEFAULT_MATERIAL_WIREFRAME_NAME,
        constants::VULKAN_SHADER_WIREFRAME_NAME,
        MaterialProperties{true, 1.0f}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_GRID{
    constants::DEFAULT_MATERIAL_GRID_NAME, constants::VULKAN_SHADER_EDITOR_GRID_NAME,
    MaterialProperties{false, 1.0f, CullMode::NONE, SurfaceType::TRANSPARENT,
                       true, false}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_CAMERA{
    constants::DEFAULT_MATERIAL_CAMERA_NAME,
    constants::VULKAN_SHADER_WIREFRAME_NAME, MaterialProperties{true, 1.0f}};

const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_LIGHT{
    constants::DEFAULT_MATERIAL_LIGHT_NAME,
    constants::VULKAN_SHADER_WIREFRAME_NAME, MaterialProperties{true, 1.0f}};


const kirana::scene::Material kirana::scene::Material::DEFAULT_MATERIAL_OUTLINE{
    constants::DEFAULT_MATERIAL_OUTLINE_NAME,
    constants::VULKAN_SHADER_EDITOR_OUTLINE_NAME,
    MaterialProperties{false, 1.0f, CullMode::FRONT, SurfaceType::OPAQUE, false,
                       true, CompareOperation::LESS_OR_EQUAL,
                       STENCIL_PROPERTIES_READ}};

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