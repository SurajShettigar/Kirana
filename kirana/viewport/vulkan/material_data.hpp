#ifndef MATERIAL_DATA_HPP
#define MATERIAL_DATA_HPP

#include "vulkan_types.hpp"

namespace kirana::scene
{
class Material;
}

namespace kirana::viewport::vulkan
{
class Pipeline;
class PipelineLayout;
class DescriptorSetLayout;
class MaterialData
{
  private:
    const scene::Material &m_material;

    std::vector<std::unique_ptr<Pipeline>> m_pipelines;
  public:

};
} // namespace kirana::viewport::vulkan
#endif