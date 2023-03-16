#ifndef KIRANA_SCENE_OBJECT_HPP
#define KIRANA_SCENE_OBJECT_HPP

#include <string>

namespace kirana::scene::converters
{
class AssimpConverter;
} // namespace kirana::scene::converters

namespace kirana::scene
{

class Object
{
    friend class converters::AssimpConverter;
  public:
    Object() = default;
    explicit Object(std::string name) : m_name{std::move(name)} {};
    virtual ~Object() = default;

    Object(const Object &object) = default;
    Object(Object &&object) = default;
    Object& operator=(const Object &object) = default;
    Object& operator=(Object &&object) = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

  protected:
    std::string m_name;
};

} // namespace kirana::scene

#endif