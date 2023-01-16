#ifndef PUSH_CONSTANT_HPP
#define PUSH_CONSTANT_HPP

#include <vulkan/vulkan.hpp>

namespace kirana::viewport::vulkan
{

class PushConstantBase
{
  public:
    [[nodiscard]] virtual inline const vk::PushConstantRange &getRange()
        const = 0;
};

template <typename T> class PushConstant : public PushConstantBase
{
  private:
    T m_current{};
    vk::PushConstantRange m_range{};

  public:
    PushConstant() = default;
    PushConstant(const T &data, vk::ShaderStageFlags stageFlags)
        : m_current{data}, m_range{vk::PushConstantRange{stageFlags, 0,
                                                         sizeof(T)}} {};
    ~PushConstant() = default;

    PushConstant(const PushConstant<T> &p)
    {
        if (this != &p)
        {
            m_current = p.m_current;
            m_range = p.m_range;
        }
    }

    PushConstant<T> &operator=(const PushConstant<T> &p)
    {
        if (this != &p)
        {
            m_current = p.m_current;
            m_range = p.m_range;
        }
        return *this;
    }

    [[nodiscard]] inline const T &get() const
    {
        return m_current;
    }

    inline void set(const T &data)
    {
        m_current = data;
    }

    [[nodiscard]] inline const vk::PushConstantRange &getRange() const override
    {
        return m_range;
    }
};
}; // namespace kirana::viewport::vulkan

#endif