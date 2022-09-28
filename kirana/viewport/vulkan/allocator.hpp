#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory>

 namespace vma
{
 class Allocator;
 }

namespace kirana::viewport::vulkan
{
class Instance;
class Device;
class Allocator
{
  private:
    bool m_isInitialized = false;
        std::unique_ptr<vma::Allocator> m_current;

    const Instance *const m_instance;
    const Device *const m_device;

  public:
    explicit Allocator(const Instance *instance, const Device *device);
    ~Allocator();
    Allocator(const Allocator &buffers) = delete;
    Allocator &operator=(const Allocator &buffers) = delete;

    const bool &isInitialized = m_isInitialized;
    const vma::Allocator &current = *m_current;
};
} // namespace kirana::viewport::vulkan
#endif