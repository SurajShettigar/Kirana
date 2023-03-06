#include "texture.hpp"

#include <assimp/texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <utility>
#include <file_system.hpp>

kirana::scene::Texture::Texture(std::string filepath, std::string name)
    : m_filepath{std::move(filepath)}, m_name{std::move(name)}
{
    if (m_name.empty())
    {
        auto fName = utils::filesystem::getFilename(m_filepath);
        m_name = fName.first + fName.second;
    }
}

kirana::scene::Texture::Texture(const aiTexture *texture)
    : m_filepath{texture->mFilename.C_Str()}, m_name{""}
{
    if (m_name.empty())
    {
        auto fName = utils::filesystem::getFilename(m_filepath);
        m_name = fName.first + fName.second;
    }
}

kirana::scene::Texture::~Texture()
{
    free();
}


void *kirana::scene::Texture::load(Channels channels, ColorSpace colorSpace)
{
    free();
    if (!utils::filesystem::fileExists(m_filepath))
        return m_pixelData;

    m_channels = channels;
    m_colorSpace = colorSpace;

    const int desiredChannels = static_cast<int>(m_channels);
    int fileChannels = 0;
    if (colorSpace == ColorSpace::sRGB)
        m_pixelData = reinterpret_cast<void *>(
            stbi_load(m_filepath.c_str(), &m_size[0], &m_size[1], &fileChannels,
                      desiredChannels));
    else
        m_pixelData = reinterpret_cast<void *>(
            stbi_loadf(m_filepath.c_str(), &m_size[0], &m_size[1],
                       &fileChannels, desiredChannels));

    m_channels = static_cast<Channels>(fileChannels);
    return m_pixelData;
}

void kirana::scene::Texture::free()
{
    if (m_pixelData != nullptr)
        stbi_image_free(m_pixelData);
}