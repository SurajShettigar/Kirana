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
        : m_name{std::move(name)}, m_id{
                                       utils::IDManager::get().generateUniqueID(
                                           m_name)} {};
    virtual ~Object() = default;

    Object(const Object &object) = default;
    Object(Object &&object) = default;
    Object &operator=(const Object &object) = default;
    Object &operator=(Object &&object) = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

    [[nodiscard]] inline const ObjectID &getID() const
    {
        return m_id;
    }

    inline void setName(const std::string &name)
    {
        m_name = name;
    }

  protected:
    std::string m_name;
    ObjectID m_id;
};

} // namespace kirana::scene

#endif