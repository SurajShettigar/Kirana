#ifndef KIRANA_SCENE_OBJECT_HPP
#define KIRANA_SCENE_OBJECT_HPP

#include <string>
#include "scene_types.hpp"

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
    explicit Object(std::string name)
        : m_name{utils::IDManager::get().generateUniqueID(std::move(name))} {};
    virtual ~Object() = default;

    Object(const Object &object) = default;
    Object(Object &&object) = default;
    Object &operator=(const Object &object) = default;
    Object &operator=(Object &&object) = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name.value;
    }

    inline void setName(const std::string &name)
    {
        m_name = utils::IDManager::get().generateUniqueID(std::move(name));
    }

  protected:
    ObjectID m_name;
};

} // namespace kirana::scene

#endif