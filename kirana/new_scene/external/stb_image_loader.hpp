#ifndef KIRANA_SCENE_EXTERNAL_STB_IMAGE_LOADER_HPP
#define KIRANA_SCENE_EXTERNAL_STB_IMAGE_LOADER_HPP

#include "image_loader.hpp"

namespace kirana::scene::external
{
class STBImageLoader : public ImageLoader
{
  public:
    STBImageLoader(const STBImageLoader &) = delete;

    static STBImageLoader &get()
    {
        static STBImageLoader instance;
        return instance;
    }
    bool loadImage(scene::Image *image, void *pixelData) override;
    void freeImage(void *pixelData) override;

  protected:
    STBImageLoader() = default;
    ~STBImageLoader() = default;
};
} // namespace kirana::scene::external
#endif