#include "app.hpp"
#include <constants.h>
#include <file_system.hpp>
#include <scene_manager.hpp>

using namespace std::placeholders;

namespace constants = kirana::utils::constants;
namespace input = kirana::window::input;


#ifdef COMPILE_BINDINGS
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

void kirana::Application::onWindowClosed(Window *window)
{
    if (window == m_viewportWindow.get())
    {
        m_logger.log(constants::LOG_CHANNEL_APPLICATION,
                     utils::LogSeverity::debug,
                     "Viewport Window closed. Cleaning viewport...");
        m_viewport.clean();
        m_isViewportRunning = false;
    }
}

void kirana::Application::onKeyboardInput(input::KeyboardInput input)
{
    if (input.action == input::KeyAction::DOWN &&
        input.key == input::Key::ESCAPE)
    {
        if (m_windowManager.isAnyWindowOpen())
            m_windowManager.closeAllWindows();
    }
}

kirana::Application::Application()
    : m_logger{kirana::utils::Logger::get()},
      m_sceneManager{kirana::scene::SceneManager::get()}
{
#if DEBUG
    m_logger.setMinSeverity(utils::LogSeverity::debug);
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
    m_windowCloseListener = m_windowManager.addOnWindowCloseListener(
        std::bind(&Application::onWindowClosed, this, _1));
    m_allWindowCloseListener = m_windowManager.addOnAllWindowsClosedListener(
        [=]() { m_isRunning = false; });
    m_keyboardInputListener = m_windowManager.addOnKeyboardInputEventListener(
        std::bind(&Application::onKeyboardInput, this, _1));

    m_viewportWindow = m_windowManager.createWindow();

    const scene::Scene &scene = m_sceneManager.loadScene();
    if (scene.isInitialized())
    {
        scene.updateCameraResolution(m_viewportWindow->getWindowResolution());
        m_logger.log(constants::LOG_CHANNEL_APPLICATION,
                     utils::LogSeverity::debug,
                     "Loaded default scene: " + scene.getRoot()->getName());
    }
    else
    {
        m_logger.log(constants::LOG_CHANNEL_APPLICATION,
                     utils::LogSeverity::error, "Failed to load default scene");
    }
    m_sceneManager.init();

    m_viewport.init(m_viewportWindow, scene);
    m_isViewportRunning = true;

    m_isRunning = true;
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application initialized");
}

void kirana::Application::update()
{
    m_windowManager.update();
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

    m_windowManager.removeOnWindowCloseListener(m_windowCloseListener);
    m_windowManager.removeOnAllWindowsClosedListener(m_allWindowCloseListener);
    m_windowManager.removeOnKeyboardInputEventListener(m_keyboardInputListener);
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
    }

    clean();
}

/**
 * @brief Construct Python Binding for Application
 *
 */
#ifdef COMPILE_BINDINGS
PYBIND11_MODULE(kirana_app, m_current)
{
    py::class_<kirana::Application>(m_current, "Application")
        .def(py::init())
        .def("run", &kirana::Application::run);
}
#endif
