#ifndef MATERIAL_DATA_HPP
#define MATERIAL_DATA_HPP

#include "vulkan_types.hpp"

namespace kirana::scene
{
class Material;
}

namespace kirana::viewport::vulkan
{
class Device;
class Pipeline;
class Shader;
class MaterialData
{
  private:
    const scene::Material &m_material;
    std::vector<const Shader *> m_shaders;
    std::vector<const Pipeline *> m_pipelines;
  public:
    MaterialData(const Device *device, const scene::Material &material);
    ~MaterialData();
    MaterialData(const MaterialData &materialData) = delete;
    MaterialData &operator=(const MaterialData &materialData) = delete;

};
} // namespace kirana::viewport::vulkan
#endif