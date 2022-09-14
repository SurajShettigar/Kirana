#include <iostream>
#include <functional>

#include "app.hpp"

using std::cerr;
using std::cout;
using std::endl;
using namespace std::placeholders;


#ifdef COMPILE_BINDINGS
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

void kirana::Application::onWindowClosed(Window *window)
{
    if(window == m_viewportWindow.get())
    {
        cout << "Viewport Window Closed" << endl;
        m_viewport.clean();
        m_isViewportRunning = false;
    }
}

kirana::Application::Application()
{
    cout << "Hello Application!" << endl;
}

kirana::Application::~Application()
{
    cout << "Goodbye Application!" << endl;
}

void kirana::Application::init()
{
    m_windowManager.init();
    m_windowCloseListener = m_windowManager.addOnWindowCloseListener(
        std::bind(&Application::onWindowClosed, this, _1));
    m_allWindowCloseListener = m_windowManager.addOnAllWindowsClosedListener(
        [=]() { m_isRunning = false; });

    m_viewportWindow = m_windowManager.createWindow();
    m_viewport.init(m_viewportWindow);

    m_isRunning = true;
    m_isViewportRunning = true;
}

void kirana::Application::update()
{
    m_windowManager.update();
    if(m_isViewportRunning)
        m_viewport.update();
}

void kirana::Application::render()
{
    if(m_isViewportRunning)
        m_viewport.render();
}

void kirana::Application::clean()
{
    m_viewport.clean();

    m_windowManager.removeOnAllWindowsClosedListener(m_allWindowCloseListener);
    m_windowManager.clean();
    m_isViewportRunning = false;
    m_isRunning = false;
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
PYBIND11_MODULE(kirana_app, m)
{
    py::class_<kirana::Application>(m, "Application")
        .def(py::init())
        .def("run", &kirana::Application::run);
}
#endif
