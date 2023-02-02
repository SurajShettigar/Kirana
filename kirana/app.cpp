#include "app.hpp"
#include <constants.h>
#include <file_system.hpp>
#include <scene_manager.hpp>

using namespace std::placeholders;

namespace constants = kirana::utils::constants;

void kirana::Application::onWindowResized(const kirana::window::Window *window,
                                          std::array<uint32_t, 2> resolution)
{
    if (window == m_viewportWindow.get())
    {
        m_sceneManager.getViewportScene().setCameraResolution(resolution);
    }
}


void kirana::Application::onWindowClosed(const Window *window)
{
    if (window == m_viewportWindow.get())
    {
        m_logger.log(constants::LOG_CHANNEL_APPLICATION,
                     utils::LogSeverity::trace,
                     "Viewport Window closed. Cleaning viewport...");
        m_viewport.clean();
        m_isViewportRunning = false;
    }
}

void kirana::Application::onKeyboardInput(KeyboardInput input)
{
    m_inputManager.m_callKeyboardEvent(input);
    if (input.action == KeyAction::DOWN)
    {
        if (input.key == Key::ESCAPE)
            if (m_windowManager.isAnyWindowOpen())
                m_windowManager.closeAllWindows();
//        if (input.key == Key::W)
//            m_viewport.toggleWireframe();
//        if (input.key == Key::R)
//            m_viewport.toggleRaytracePBR();
    }
}

void kirana::Application::onMouseInput(MouseInput input)
{
    m_inputManager.m_callMouseEvent(input);
}

void kirana::Application::onScrollInput(double xOffset, double yOffset)
{
    m_inputManager.m_callScrollEvent(xOffset, yOffset);
}

kirana::Application::Application()
    : m_logger{kirana::utils::Logger::get()},
      m_inputManager{utils::input::InputManager::get()},
      m_time{kirana::utils::Time::get()},
      m_sceneManager{kirana::scene::SceneManager::get()}
{
#if DEBUG
    m_logger.setMinSeverity(utils::LogSeverity::trace);
#else
    m_logger.setMinSeverity(utils::LogSeverity::info);
#endif

    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application created");
}

kirana::Application::~Application()
{
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application destroyed");
}

void kirana::Application::init()
{
    m_windowManager.init();
    if (!m_windowManager.isInitialized)
    {
        m_logger.log(
            constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::error,
            "Application failed to initialize. Window Manager not initialized");
        return;
    }

    m_windowResizeListener = m_windowManager.addOnWindowResizeListener(
        std::bind(&Application::onWindowResized, this, _1, _2));
    m_windowCloseListener = m_windowManager.addOnWindowCloseListener(
        std::bind(&Application::onWindowClosed, this, _1));
    m_allWindowCloseListener = m_windowManager.addOnAllWindowsClosedListener(
        [=]() { m_isRunning = false; });
    m_keyboardInputListener = m_windowManager.addOnKeyboardInputEventListener(
        std::bind(&Application::onKeyboardInput, this, _1));
    m_mouseInputListener = m_windowManager.addOnMouseInputEventListener(
        std::bind(&Application::onMouseInput, this, _1));
    m_scrollInputListener = m_windowManager.addOnScrollInputEventListener(
        std::bind(&Application::onScrollInput, this, _1, _2));

    if (m_windowPointer)
        m_viewportWindow =
            m_windowManager.createWindow(m_windowPointer, "Kirana", false,
                                         false, m_windowWidth, m_windowHeight);
    else
        m_viewportWindow = m_windowManager.createWindow("Kirana", false, true);

    m_sceneManager.init();

    scene::ViewportScene &scene = m_sceneManager.getViewportScene();
    scene.setCameraResolution(m_viewportWindow->resolution);

    m_viewport.init(m_viewportWindow.get(), scene);
    m_isViewportRunning = true;

    if (m_sceneManager.loadScene())
    {
        m_logger.log(
            constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::debug,
            "Loaded default scene: " + scene.getCurrentScene().getName());
    }
    else
    {
        m_logger.log(constants::LOG_CHANNEL_APPLICATION,
                     utils::LogSeverity::error, "Failed to load default scene");
    }

    m_isRunning = true;
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application initialized");
}

void kirana::Application::update()
{
    m_time.update();
    m_windowManager.update();
    // TODO: Update input manager keyboard and mouse clicks every frame.
    m_inputManager.m_isMouseInside =
        m_windowManager.getCurrentWindow()->isCursorInside;
    m_inputManager.m_updateMousePosition(
        m_windowManager.getCurrentWindow()->cursorPosition);
    m_sceneManager.update();
    if (m_isViewportRunning)
        m_viewport.update();
}

void kirana::Application::render()
{
    if (m_isViewportRunning)
        m_viewport.render();
}

void kirana::Application::clean()
{
    if (m_isViewportRunning)
    {
        m_viewport.clean();
        m_isViewportRunning = false;
    }

    m_windowManager.removeOnWindowResizeListener(m_windowResizeListener);
    m_windowManager.removeOnWindowCloseListener(m_windowCloseListener);
    m_windowManager.removeOnAllWindowsClosedListener(m_allWindowCloseListener);
    m_windowManager.removeOnKeyboardInputEventListener(m_keyboardInputListener);
    m_windowManager.removeOnMouseInputEventListener(m_mouseInputListener);
    m_windowManager.removeOnScrollInputEventListener(m_scrollInputListener);
    m_windowManager.clean();

    m_sceneManager.clean();

    m_isRunning = false;

    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application cleaned");
}

void kirana::Application::run()
{
    init();

    while (m_isRunning)
    {
        update();
        render();
        //        std::cout << "FPS: " << m_time.getFPS() << "\r" << std::flush;
    }

    clean();
}

void kirana::Application::run(long windowId, int width, int height)
{
    m_windowPointer = windowId;
    m_windowWidth = width;
    m_windowHeight = height;
    run();
}
