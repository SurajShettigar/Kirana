#ifndef KIRANA_SCENE_IMAGE_HPP
#define KIRANA_SCENE_IMAGE_HPP

#include "object.hpp"
#include "image_types.hpp"
#include <string>

namespace kirana::scene::converters
{
class STBImageConverter;
}

namespace kirana::scene
{
class Image: public Object
{
    friend class converters::STBImageConverter;
    friend class converters::AssimpConverter;
  public:
    Image() = default;
    explicit Image(std::string filepath, ImageProperties properties);
    ~Image() override = default;

    Image(const Image &image) = default;
    Image(Image &&image) = default;
    Image &operator=(const Image &image) = default;
    Image &operator=(Image &&image) = default;

    [[nodiscard]] inline const std::string &getFilepath() const
    {
        return m_filepath;
    }

    [[nodiscard]] inline const std::array<int, 2> &getSize() const
    {
        return m_size;
    }

    [[nodiscard]] inline const ImageProperties &getProperties() const
    {
        return m_properties;
    }

    [[nodiscard]] inline size_t getPixelDataSize() const
    {
        return static_cast<size_t>(m_size[0] * m_size[1]) *
               static_cast<size_t>(m_properties.channels);
    }

  protected:
    std::string m_filepath;
    std::array<int, 2> m_size = {0, 0};
    ImageProperties m_properties;
};
}; // namespace kirana::scene

#endif
