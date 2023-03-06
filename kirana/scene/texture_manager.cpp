#include "texture_manager.hpp"

#include <file_system.hpp>
#include <assimp/texture.h>


int kirana::scene::TextureManager::addTexture(const std::string &filepath,
                                              const std::string &name)
{
    if (!kirana::utils::filesystem::fileExists(filepath))
        return -1;

    if (m_textureIndexTable.find(filepath) != m_textureIndexTable.end())
        return static_cast<int>(m_textureIndexTable.at(filepath));

    m_textures.emplace_back(
        std::move(std::make_unique<Texture>(filepath, name)));

    const auto &tex = *m_textures.back();
    const auto texIndex = static_cast<uint32_t>(m_textures.size() - 1);
    m_textureIndexTable[filepath] = texIndex;
    return static_cast<int>(texIndex);
}