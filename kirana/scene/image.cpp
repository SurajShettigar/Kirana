#include "image.hpp"

#include <assimp/texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <utility>
#include <file_system.hpp>

kirana::scene::Image::Image(std::string filepath, std::string name,
                            uint32_t index, ImageProperties properties)
    : m_filepath{std::move(filepath)}, m_name{std::move(name)}, m_index{index},
      m_properties{std::move(properties)}
{
    if (m_name.empty())
    {
        auto fName = utils::filesystem::getFilename(m_filepath);
        m_name = fName.first + fName.second;
    }
}

kirana::scene::Image::Image(const aiTexture *texture, uint32_t index,
                            ImageProperties properties)
    : m_filepath{texture->mFilename.C_Str()}, m_name{""}, m_index{index},
      m_properties{std::move(properties)}
{
    if (m_name.empty())
    {
        auto fName = utils::filesystem::getFilename(m_filepath);
        m_name = fName.first + fName.second;
    }
}

kirana::scene::Image::~Image()
{
    free();
}

void *kirana::scene::Image::load()
{
    free();
    if (!utils::filesystem::fileExists(m_filepath))
        return m_pixelData;

    const int desiredChannels = static_cast<int>(m_properties.channels);
    int fileChannels = 0;
    if (m_properties.colorSpace == ColorSpace::sRGB)
        m_pixelData = reinterpret_cast<void *>(
            stbi_load(m_filepath.c_str(), &m_size[0], &m_size[1], &fileChannels,
                      desiredChannels));
    else
        m_pixelData = reinterpret_cast<void *>(
            stbi_loadf(m_filepath.c_str(), &m_size[0], &m_size[1],
                       &fileChannels, desiredChannels));

    m_properties.fileChannels = static_cast<Channels>(fileChannels);
    return m_pixelData;
}

void kirana::scene::Image::free()
{
    if (m_pixelData != nullptr)
    {
        stbi_image_free(m_pixelData);
        m_pixelData = nullptr;
    }
}