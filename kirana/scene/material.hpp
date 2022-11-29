#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>

namespace kirana::scene
{
class Material
{
  private:
    std::string m_name = "Material";
    std::string m_shader = "MatCap";
  public:
    Material() = default;
    explicit Material(std::string name, std::string shader="");
    ~Material() = default;

    Material(const Material &material);
    Material &operator=(const Material &material);

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::string &getShader() const
    {
        return m_shader;
    }
};
} // namespace kirana::scene

#endif