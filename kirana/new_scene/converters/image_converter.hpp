#ifndef KIRANA_SCENE_CONVERTERS_IMAGE_CONVERTER_HPP
#define KIRANA_SCENE_CONVERTERS_IMAGE_CONVERTER_HPP


namespace kirana::scene
{
class Image;
} // namespace kirana::scene

namespace kirana::scene::converters
{
class ImageConverter
{
  public:
    ImageConverter(const ImageConverter &imageConverter) = delete;

    virtual bool loadImage(scene::Image *image, void *pixelData) = 0;
    virtual void freeImage(void *pixelData) = 0;

  protected:
    ImageConverter() = default;
    ~ImageConverter() = default;
};
} // namespace kirana::scene::converters
#endif // KIRANA_SCENE_CONVERTERS_CONVERTER_HPP