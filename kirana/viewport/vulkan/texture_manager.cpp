#include "texture_manager.hpp"
#include "texture.hpp"
#include "texture_sampler.hpp"
#include "vulkan_utils.hpp"

#include <image.hpp>

kirana::viewport::vulkan::TextureManager::TextureManager(
    const Device *device, const Allocator *allocator)
    : m_device{device}, m_allocator{allocator}
{
}

kirana::viewport::vulkan::TextureManager::~TextureManager()
{
    for (auto &t : m_textures)
        if (t)
            delete t;
    for (auto &s : m_samplers)
        if (s)
            delete s;
    m_samplers.clear();
    m_textures.clear();
}

const kirana::viewport::vulkan::TextureSampler *kirana::viewport::vulkan::
    TextureManager::getSampler(const scene::ImageProperties &imageProperties)
{
    const TextureSampler::Properties samplerProps{
        static_cast<TextureSampler::Filter>(imageProperties.magFilter),
        static_cast<TextureSampler::Filter>(imageProperties.minFilter),
        static_cast<TextureSampler::MipMapFilter>(imageProperties.mipMapFilter),
        {static_cast<TextureSampler::WrapMode>(imageProperties.uvwWrapMode[0]),
         static_cast<TextureSampler::WrapMode>(imageProperties.uvwWrapMode[1]),
         static_cast<TextureSampler::WrapMode>(imageProperties.uvwWrapMode[2])},
        static_cast<float>(imageProperties.mipLODBias),
        imageProperties.enableAnisotropicFiltering,
        static_cast<float>(imageProperties.anisotropicLevel)};

    auto it = std::find_if(m_samplers.begin(), m_samplers.end(),
                           [&samplerProps](const auto &sampler) {
                               return sampler->getProperties() == samplerProps;
                           });

    if (it != m_samplers.end())
        return *it;

    m_samplers.emplace_back(new TextureSampler(m_device, samplerProps));
    return m_samplers.back();
}

vk::Format kirana::viewport::vulkan::TextureManager::getTextureFormat(
    scene::Channels channels, scene::ColorSpace colorSpace) const
{
    switch (channels)
    {
    case scene::Channels::GRAYSCALE:
        return colorSpace == scene::ColorSpace::sRGB ? vk::Format::eR8Srgb
                                                     : vk::Format::eR16Sfloat;
    case scene::Channels::GRAYSCALE_ALPHA:
        return colorSpace == scene::ColorSpace::sRGB
                   ? vk::Format::eR8G8Srgb
                   : vk::Format::eR16G16Sfloat;
    case scene::Channels::RGB:
        return colorSpace == scene::ColorSpace::sRGB
                   ? vk::Format::eR8G8B8Srgb
                   : vk::Format::eR16G16B16Sfloat;
    case scene::Channels::RGBA:
    default:
        return colorSpace == scene::ColorSpace::sRGB
                   ? vk::Format::eR8G8B8A8Srgb
                   : vk::Format::eR16G16B16A16Sfloat;
    }
}

bool kirana::viewport::vulkan::TextureManager::addTexture(scene::Image *image)
{
    if (m_textureIndexTable.find(image->getFilepath()) !=
        m_textureIndexTable.end())
        return true;

    void *pixelData = image->load();
    if (pixelData == nullptr)
        return false;
    const size_t pixelDataSize = image->getPixelDataSize();

    const scene::ImageProperties imageProps = image->getProperties();
    const auto &sampler = getSampler(imageProps);

    // TODO: Add Mip-Mapping

    const std::array<int, 2> &texSize = image->getSize();
    const Texture::Properties texProps{
        {static_cast<uint32_t>(texSize[0]), static_cast<uint32_t>(texSize[1]),
         1},
        getTextureFormat(imageProps.channels, imageProps.colorSpace),
        vk::ImageUsageFlagBits::eSampled,
        vk::ImageAspectFlagBits::eColor,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageType::e2D};

    m_textures.emplace_back(
        new Texture(m_device, m_allocator, texProps, sampler, image->getName(),
                    image->getIndex(), pixelData, pixelDataSize));
    image->free();
    if (m_textures.back()->isInitialized)
    {
        const uint32_t texIndex = static_cast<uint32_t>(m_textures.size() - 1);
        m_textureIndexTable[image->getFilepath()] = texIndex;
        return true;
    }
    else
        return false;
}

const std::vector<kirana::viewport::vulkan::Texture *>
    &kirana::viewport::vulkan::TextureManager::getTextures()
{
    int i = 0;
    std::sort(m_textures.begin(), m_textures.end(), [](Texture *a, Texture *b) {
        return a->getIndex() < b->getIndex();
    });
    return m_textures;
}