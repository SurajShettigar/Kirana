#ifndef KIRANA_SCENE_IMAGE_MANAGER_HPP
#define KIRANA_SCENE_IMAGE_MANAGER_HPP

#include "image.hpp"
#include <unordered_map>

namespace kirana::scene
{
class ImageManager
{
  public:
    ImageManager(const ImageManager &imageManager) = delete;

    static ImageManager &get()
    {
        static ImageManager instance;
        return instance;
    }

    [[nodiscard]] inline const Image &getImage(uint32_t index) const
    {
        return m_images.at(index);
    }

    [[nodiscard]] inline const Image &getImage(const std::string &name) const
    {
        return getImage(m_imageIndexTable.at(name));
    }

    int addImage(const std::string &basePath, const std::string &imagePath,
                 const ImageProperties &properties = {});

    bool loadImage(uint32_t imageIndex, void *pixelData, size_t *pixelDataSize);
    inline bool loadImage(const std::string &name, void *pixelData,
                          size_t *pixelDataSize)
    {
        return loadImage(m_imageIndexTable.at(name), pixelData, pixelDataSize);
    }
    void freeImage(uint32_t imageIndex);
    inline void freeImage(const std::string &name)
    {
        freeImage(m_imageIndexTable.at(name));
    }

    void removeImage(uint32_t index)
    {
        if (index < m_images.size())
        {
            freeImage(index);
            const std::string &name = m_images[index].getName();
            m_imageIndexTable.erase(name);
            m_images.erase(m_images.begin() + index);
        }
    }

    inline void removeImage(const std::string &name)
    {
        if (m_imageIndexTable.find(name) != m_imageIndexTable.end())
            removeImage(m_imageIndexTable.at(name));
    }

  private:
    ImageManager() = default;
    ~ImageManager();

    std::unordered_map<std::string, uint32_t> m_imageIndexTable;
    std::vector<Image> m_images;
    std::unordered_map<uint32_t, void *> m_loadedImageTable;
};
} // namespace kirana::scene
#endif