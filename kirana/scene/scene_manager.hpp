#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "scene.hpp"

#include <input_manager.hpp>
#include <time.hpp>
#include <string>

namespace kirana::scene
{
class SceneManager
{
  private:
    struct ViewportCameraData
    {
        math::Vector2 prevMousePos;
        math::Vector2 mousePos;
        math::Vector2 mouseDelta;
        bool firstClick = true;
        utils::input::MouseButton currentButton;

        [[nodiscard]] math::Vector2 normalizedPrevMousePos(
            const std::array<uint32_t, 2> windowSize) const
        {
            math::Vector2 pos = math::Vector2::normalize(math::Vector2(
                prevMousePos[0] / static_cast<float>(windowSize[0]),
                prevMousePos[1] / static_cast<float>(windowSize[0])));
            pos[0] = pos[0] * 2.0f - 1.0f;
            pos[1] = pos[1] * 2.0f - 1.0f;
            return pos;
        }

        [[nodiscard]] math::Vector2 normalizedMousePos(
            const std::array<uint32_t, 2> windowSize) const
        {
            math::Vector2 pos = math::Vector2::normalize(
                math::Vector2(mousePos[0] / static_cast<float>(windowSize[0]),
                              mousePos[1] / static_cast<float>(windowSize[0])));
            pos[0] = pos[0] * 2.0f - 1.0f;
            pos[1] = pos[1] * 2.0f - 1.0f;
            return pos;
        }
    };

    Scene m_currentScene;
    PerspectiveCamera &m_viewportCamera;

    utils::input::InputManager &m_inputManager;
    utils::Time &m_time;

    ViewportCameraData m_viewportCamData;

    void handleViewportCameraInput();
    SceneManager();
    ~SceneManager() = default;

  public:
    SceneManager(const SceneManager &manager) = delete;

    static SceneManager &get()
    {
        static SceneManager instance;
        return instance;
    }

    Scene &loadScene(std::string path = "",
                     const SceneImportSettings &importSettings =
                         DEFAULT_SCENE_IMPORT_SETTINGS);
    inline Scene &getCurrentScene()
    {
        return m_currentScene;
    }

    void init();
    void update();
    void clean();
};
} // namespace kirana::scene
#endif