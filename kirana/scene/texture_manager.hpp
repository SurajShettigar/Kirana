#ifndef KIRANA_SCENE_TEXTURE_MANAGER_HPP
#define KIRANA_SCENE_TEXTURE_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <memory>

#include "texture.hpp"

struct aiTexture;

namespace kirana::scene
{
class TextureManager
{

  public:
    TextureManager(const TextureManager &textureManager) = delete;

    static TextureManager &get()
    {
        static TextureManager instance;
        return instance;
    }

    [[nodiscard]] inline const Texture *getTexture(uint32_t index) const
    {
        return index < m_textures.size() ? m_textures[index].get() : nullptr;
    }

    [[nodiscard]] inline const Texture *getTexture(
        const std::string &path) const
    {
        return m_textureIndexTable.find(path) != m_textureIndexTable.end()
                   ? getTexture(m_textureIndexTable.at(path))
                   : nullptr;
    }

    int addTexture(const std::string &filepath,
                   const std::string &name = "");

    void removeTexture(uint32_t index)
    {
        if (index < m_textures.size())
        {
            const std::string &name = m_textures[index]->getName();
            m_textureIndexTable.erase(name);
            m_textures.erase(m_textures.begin() + index);
        }
    }

    inline void removeTexture(const std::string &path)
    {
        if (m_textureIndexTable.find(path) != m_textureIndexTable.end())
            removeTexture(m_textureIndexTable.at(path));
    }

  private:
    TextureManager() = default;
    ~TextureManager() = default;

    std::unordered_map<std::string, uint32_t> m_textureIndexTable;
    std::vector<std::unique_ptr<Texture>> m_textures;
};
} // namespace kirana::scene

#endif // KIRANA_SCENE_TEXTURE_MANAGER_HPP
