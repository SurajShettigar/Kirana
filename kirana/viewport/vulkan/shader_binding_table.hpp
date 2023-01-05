#ifndef SHADER_BINDING_TABLE_HPP
#define SHADER_BINDING_TABLE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class ShaderBindingTable
{
  private:
    AllocatedBuffer m_buffer;

  public:
    ShaderBindingTable();
    ~ShaderBindingTable();

    ShaderBindingTable(const ShaderBindingTable &table) = delete;
    ShaderBindingTable &operator=(const ShaderBindingTable &table) = delete;
};
} // namespace kirana::viewport::vulkan
#endif