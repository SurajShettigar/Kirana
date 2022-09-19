#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "vulkan_utils.hpp"

namespace kirana::viewport::vulkan
{
class Instance
{
  private:
    vk::Instance m_current;
    vk::DebugUtilsMessengerEXT m_debugMessenger;

  public:
    const vk::Instance &current = m_current;
    const vk::DebugUtilsMessengerEXT &debugMessenger = m_debugMessenger;

    explicit Instance(const std::vector<const char *> &reqInstanceExtensions);
    ~Instance();

    Instance(const Instance &device) = delete;
    Instance &operator=(const Instance &device) = delete;
};
} // namespace kirana::viewport::vulkan


#endif