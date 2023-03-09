#include "image_manager.hpp"

#include <file_system.hpp>
#include <assimp/texture.h>


int kirana::scene::ImageManager::addImage(const std::string &filepath,
                                          const std::string &name,
                                          const ImageProperties &properties)
{
    // TODO: Use incremental names instead of filepaths as keys.
    if (!kirana::utils::filesystem::fileExists(filepath))
        return -1;

    if (m_imageIndexTable.find(filepath) != m_imageIndexTable.end())
        return static_cast<int>(m_imageIndexTable.at(filepath));

    const auto imgIndex = static_cast<uint32_t>(m_images.size());
    m_images.emplace_back(std::move(
        std::make_unique<Image>(filepath, name, imgIndex, properties)));
    m_imageIndexTable[filepath] = imgIndex;
    return static_cast<int>(imgIndex);
}