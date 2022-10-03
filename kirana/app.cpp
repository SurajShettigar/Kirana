#include "app.hpp"
#include <constants.h>
#include <file_system.hpp>
#include <scene_importer.hpp>

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

kirana::Application::Application() : m_logger{kirana::utils::Logger::get()}
{
    m_logger.setMinSeverity(utils::LogSeverity::debug);
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application created");
}

kirana::Application::~Application()
{
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::trace,
                 "Application destroyed");
}

bool kirana::Application::loadDefaultScene()
{
    std::string filePath = utils::filesystem::combinePath(
        constants::DATA_DIR_PATH, {constants::DEFAULT_MODEL_NAME});

    if (scene::SceneImporter::get().loadSceneFromFile(
            filePath.c_str(), scene::DEFAULT_SCENE_IMPORT_SETTINGS,
            &m_currentScene))
    {
        m_logger.log(
            constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::debug,
            "Loaded default scene: " + m_currentScene.rootObject->name);
        return true;
    }
    else
    {
        m_logger.log(
            constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::error,
            "Failed to load default scene");
        return false;
    }
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
    if (loadDefaultScene())
    {
        m_viewport.init(m_viewportWindow, m_currentScene);
        m_isViewportRunning = true;
    }

    m_isRunning = true;
    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::debug,
                 "Application initialized");
}

void kirana::Application::update()
{
    m_windowManager.update();
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
    m_windowManager.clean();

    m_isRunning = false;

    m_logger.log(constants::LOG_CHANNEL_APPLICATION, utils::LogSeverity::debug,
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
