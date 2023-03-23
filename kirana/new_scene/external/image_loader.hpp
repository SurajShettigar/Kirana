#ifndef KIRANA_SCENE_EXTERNAL_IMAGE_LOADER_HPP
#define KIRANA_SCENE_EXTERNAL_IMAGE_LOADER_HPP


namespace kirana::scene
{
class Image;
} // namespace kirana::scene

namespace kirana::scene::external
{
class ImageLoader
{
  public:
    ImageLoader(const ImageLoader &imageLoader) = delete;

    virtual bool loadImage(scene::Image *image, void *pixelData) = 0;
    virtual void freeImage(void *pixelData) = 0;
  protected:
    ImageLoader() = default;
    ~ImageLoader() = default;
};
} // namespace kirana::scene::converters
#endif // KIRANA_SCENE_CONVERTERS_CONVERTER_HPP