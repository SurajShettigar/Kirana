#ifndef KIRANA_SCENE_IMAGE_MANAGER_HPP
#define KIRANA_SCENE_IMAGE_MANAGER_HPP

#include "image.hpp"

struct aiTexture;

namespace kirana::scene
{
class ImageManager
{
  public:
    ImageManager(const ImageManager &textureManager) = delete;

    static ImageManager &get()
    {
        static ImageManager instance;
        return instance;
    }

    [[nodiscard]] inline Image *getImage(uint32_t index)
    {
        return index < m_images.size() ? m_images[index].get() : nullptr;
    }

    [[nodiscard]] inline Image *getImage(const std::string &path)
    {
        return m_imageIndexTable.find(path) != m_imageIndexTable.end()
                   ? getImage(m_imageIndexTable.at(path))
                   : nullptr;
    }

    int addImage(const std::string &filepath, const std::string &name = "", const ImageProperties &properties = {});

    void removeImage(uint32_t index)
    {
        if (index < m_images.size())
        {
            const std::string &name = m_images[index]->getName();
            m_imageIndexTable.erase(name);
            m_images.erase(m_images.begin() + index);
        }
    }

    inline void removeImage(const std::string &path)
    {
        if (m_imageIndexTable.find(path) != m_imageIndexTable.end())
            removeImage(m_imageIndexTable.at(path));
    }

  private:
    ImageManager() = default;
    ~ImageManager() = default;

    std::unordered_map<std::string, uint32_t> m_imageIndexTable;
    std::vector<std::unique_ptr<Image>> m_images;
};
} // namespace kirana::scene

#endif // KIRANA_SCENE_IMAGE_MANAGER_HPP
