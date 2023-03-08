#ifndef KIRANA_VIEWPORT_VULKAN_TEXTURE_MANAGER_HPP
#define KIRANA_VIEWPORT_VULKAN_TEXTURE_MANAGER_HPP

#include "vulkan_types.hpp"

namespace kirana::scene
{
enum class Channels;
enum class ColorSpace;
struct ImageProperties;
class Image;
} // namespace kirana::scene

namespace kirana::viewport::vulkan
{

class Device;
class Allocator;
class Texture;
class TextureSampler;

class TextureManager
{
  public:
    TextureManager(const Device *device, const Allocator *allocator);
    ~TextureManager();

    TextureManager(const TextureManager &textureManager) = delete;
    TextureManager &operator=(const TextureManager &textureManager) = delete;
    bool addTexture(scene::Image *image);

    const std::vector<Texture *> &getTextures();

  private:
    const Device *const m_device = nullptr;
    const Allocator *const m_allocator = nullptr;

    std::unordered_map<std::string, uint32_t> m_textureIndexTable;
    std::vector<Texture *> m_textures;
    std::vector<TextureSampler *> m_samplers;

    const TextureSampler *getSampler(
        const scene::ImageProperties &imageProperties);
    [[nodiscard]] vk::Format getTextureFormat(
        scene::Channels channels, scene::ColorSpace colorSpace) const;
};
} // namespace kirana::viewport::vulkan

#endif