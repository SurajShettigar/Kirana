#include "image_manager.hpp"

#include "converters/stb_image_converter.hpp"
#include <file_system.hpp>


kirana::scene::ImageManager::~ImageManager()
{
    for (const auto &i : m_loadedImageTable)
        freeImage(i.first);
}


int kirana::scene::ImageManager::addImage(const std::string &basePath,
                                          const std::string &imagePath,
                                          const ImageProperties &properties)
{
    const std::string path =
        basePath.empty()
            ? imagePath
            : utils::filesystem::combinePath(basePath, {imagePath});

    if (!utils::filesystem::fileExists(path))
        return -1;

    int imageIndex = static_cast<int>(m_images.size());
    m_images.emplace_back(std::move(Image{path, properties}));
    m_imageIndexTable[m_images.back().getName()] = imageIndex;

    return imageIndex;
}

bool kirana::scene::ImageManager::loadImage(uint32_t imageIndex,
                                            void *pixelData,
                                            size_t *pixelDataSize)
{
    if (imageIndex >= m_images.size())
        return false;

    Image &image = m_images[imageIndex];
    if (!converters::STBImageConverter::get().loadImage(&image, pixelData))
        return false;

    m_loadedImageTable[imageIndex] = pixelData;
    *pixelDataSize = image.getPixelDataSize();
    return true;
}

void kirana::scene::ImageManager::freeImage(uint32_t imageIndex)
{
    if (m_loadedImageTable.find(imageIndex) == m_loadedImageTable.end() ||
        m_loadedImageTable.at(imageIndex) == nullptr)
        return;

    converters::STBImageConverter::get().freeImage(
        m_loadedImageTable.at(imageIndex));
    m_loadedImageTable.erase(imageIndex);
}