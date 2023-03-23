#ifndef KIRANA_SCENE_OBJECT_HPP
#define KIRANA_SCENE_OBJECT_HPP

#include <string>

namespace kirana::scene::external
{
class AssimpSceneConverter;
} // namespace kirana::scene::external

namespace kirana::scene
{

class Object
{
    friend class external::AssimpSceneConverter;

  public:
    Object() = default;
    explicit Object(std::string name) : m_name{std::move(name)} {};
    virtual ~Object() = default;

    Object(const Object &object) = default;
    Object(Object &&object) = default;
    Object &operator=(const Object &object) = default;
    Object &operator=(Object &&object) = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

    inline void setName(const std::string &name)
    {
        m_name = name;
    }

  protected:
    std::string m_name;
};

} // namespace kirana::scene

#endif