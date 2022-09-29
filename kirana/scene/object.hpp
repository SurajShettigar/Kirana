#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>
#include <string>

struct aiNode;

namespace kirana::scene
{
class Mesh;
class Object
{
  private:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_parent;

  public:
    Object(const aiNode *node, const std::vector<std::shared_ptr<Mesh>> &meshes,
           const std::shared_ptr<Object> &parent = nullptr);
    ~Object() = default;

    inline void setParent(const std::shared_ptr<Object> &p)
    {
        m_parent = p;
    }

    const std::string &name = m_name;
    const std::vector<std::shared_ptr<Mesh>> &meshes = m_meshes;
    const std::shared_ptr<Object> &parent = m_parent;
};
} // namespace kirana::scene
#endif