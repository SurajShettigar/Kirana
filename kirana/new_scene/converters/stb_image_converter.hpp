#ifndef KIRANA_SCENE_CONVERTERS_STB_IMAGE_CONVERTER_HPP
#define KIRANA_SCENE_CONVERTERS_STB_IMAGE_CONVERTER_HPP

#include "image_converter.hpp"

namespace kirana::scene::converters
{
class STBImageConverter : public ImageConverter
{
  public:
    STBImageConverter(const STBImageConverter &imageConverter) = delete;

    static STBImageConverter &get()
    {
        static STBImageConverter instance;
        return instance;
    }
   bool loadImage(scene::Image *image, void *pixelData) override;
   void freeImage(void *pixelData) override;

  protected:
    STBImageConverter() = default;
    ~STBImageConverter() = default;
};
} // namespace kirana::scene::converters
#endif // KIRANA_SCENE_CONVERTERS_CONVERTER_HPP