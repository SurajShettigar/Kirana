#include "image_manager.hpp"

#include <file_system.hpp>
#include <assimp/texture.h>


int kirana::scene::ImageManager::addImage(const std::string &filepath,
                                              const std::string &name)
{
    if (!kirana::utils::filesystem::fileExists(filepath))
        return -1;

    if (m_imageIndexTable.find(filepath) != m_imageIndexTable.end())
        return static_cast<int>(m_imageIndexTable.at(filepath));

    m_images.emplace_back(
        std::move(std::make_unique<Image>(filepath, name)));

    const auto &tex = *m_images.back();
    const auto texIndex = static_cast<uint32_t>(m_images.size() - 1);
    m_imageIndexTable[filepath] = texIndex;
    return static_cast<int>(texIndex);
}