#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{
class Instance
{
  private:
    bool m_isInitialized = false;
    vk::Instance m_current;
    vk::DebugUtilsMessengerEXT m_debugMessenger;

  public:
    const bool &isInitialized = m_isInitialized;
    const vk::Instance &current = m_current;
    const vk::DebugUtilsMessengerEXT &debugMessenger = m_debugMessenger;

    explicit Instance(const std::vector<const char *> &reqInstanceExtensions);
    ~Instance();

    Instance(const Instance &device) = delete;
    Instance &operator=(const Instance &device) = delete;
};
} // namespace kirana::viewport::vulkan


#endif