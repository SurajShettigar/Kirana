#include "stb_image_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <image.hpp>
#include <file_system.hpp>

bool kirana::scene::external::STBImageLoader::loadImage(scene::Image *image,
                                                        void *pixelData)
{
    const std::string &file = image->getFilepath();
    const ImageProperties &props = image->getProperties();

    if (!utils::filesystem::fileExists(file))
        return false;

    const int desiredChannels = static_cast<int>(props.channels);
    int fileChannels = 0;
    if (props.colorSpace == ColorSpace::sRGB)
        pixelData = reinterpret_cast<void *>(
            stbi_load(file.c_str(), &image->m_size[0], &image->m_size[1],
                      &fileChannels, desiredChannels));
    else
        pixelData = reinterpret_cast<void *>(
            stbi_loadf(file.c_str(), &image->m_size[0], &image->m_size[1],
                       &fileChannels, desiredChannels));

    image->m_properties.fileChannels = static_cast<Channels>(fileChannels);
    if (pixelData == nullptr)
        return false;
    return true;
}

void kirana::scene::external::STBImageLoader::freeImage(void *pixelData)
{
    if (pixelData != nullptr)
        stbi_image_free(pixelData);
}
