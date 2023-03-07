#ifndef KIRANA_SCENE_TEXTURE_HPP
#define KIRANA_SCENE_TEXTURE_HPP

#include <string>
#include <array>
#include <vector>

struct aiTexture;

namespace kirana::scene
{

class Image
{
  public:
    enum class Channels
    {
        NONE = 0,
        GRAYSCALE = 1,
        GRAYSCALE_ALPHA = 2,
        RGB = 3,
        RGBA = 4
    };

    enum class ColorSpace
    {
        sRGB = 0,
        Linear = 1
    };

    Image(std::string filepath, std::string name);
    Image(const aiTexture *texture);
    ~Image();

    Image(const Image &texture) = delete;
    Image &operator=(const Image &texture) = delete;

    void *load(Channels channels = Channels::RGBA,
               ColorSpace colorSpace = ColorSpace::sRGB);
    void free();

    [[nodiscard]] inline const std::string &getFilepath() const
    {
        return m_filepath;
    }

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

    [[nodiscard]] inline const std::array<int, 2> &getSize() const
    {
        return m_size;
    }

    [[nodiscard]] inline const Channels &getChannels() const
    {
        return m_channels;
    }

    [[nodiscard]] inline ColorSpace getColorSpace() const
    {
        return m_colorSpace;
    }

    [[nodiscard]] inline const void *getPixelData() const
    {
        return m_pixelData;
    }

    [[nodiscard]] inline size_t getPixelDataSize() const
    {
        return static_cast<size_t>(m_size[0] * m_size[1]) *
               static_cast<size_t>(m_channels);
    }
  private:
    std::string m_filepath;
    std::string m_name;
    std::array<int, 2> m_size = {0, 0};
    Channels m_channels = Channels::RGBA;
    ColorSpace m_colorSpace = ColorSpace::sRGB;

    void *m_pixelData = nullptr;
};
}; // namespace kirana::scene

#endif
