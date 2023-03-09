#ifndef KIRANA_SCENE_TEXTURE_HPP
#define KIRANA_SCENE_TEXTURE_HPP

#include "image_types.hpp"

struct aiTexture;

namespace kirana::scene
{

class Image
{
  public:
    Image(std::string filepath, std::string name, uint32_t index,
          ImageProperties properties);
    Image(const aiTexture *texture, uint32_t index, ImageProperties properties);
    ~Image();

    Image(const Image &image) = delete;
    Image &operator=(const Image &image) = delete;

    void *load();
    void free();

    [[nodiscard]] inline const std::string &getFilepath() const
    {
        return m_filepath;
    }

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

    [[nodiscard]] inline const uint32_t getIndex() const
    {
        return m_index;
    }

    [[nodiscard]] inline const std::array<int, 2> &getSize() const
    {
        return m_size;
    }

    [[nodiscard]] inline const ImageProperties &getProperties() const
    {
        return m_properties;
    }

    [[nodiscard]] inline const void *getPixelData() const
    {
        return m_pixelData;
    }

    [[nodiscard]] inline size_t getPixelDataSize() const
    {
        return static_cast<size_t>(m_size[0] * m_size[1]) *
               static_cast<size_t>(m_properties.channels);
    }

  private:
    std::string m_filepath;
    std::string m_name;
    uint32_t m_index;
    std::array<int, 2> m_size = {0, 0};
    ImageProperties m_properties;

    void *m_pixelData = nullptr;
};
}; // namespace kirana::scene

#endif
