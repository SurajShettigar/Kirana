#ifndef INSTANCE_HPP
#define INSTANCE_HPP


#include "vulkan_utils.hpp"
namespace kirana::viewport::renderer
{
class Instance
{
  private:
    vk::Instance m_current;

    Instance(const Instance &device) = delete;
    Instance &operator=(const Instance &device) = delete;

  public:
    const vk::Instance &current = m_current;

    Instance();
    ~Instance();
};
} // namespace kirana::viewport::renderer


#endif