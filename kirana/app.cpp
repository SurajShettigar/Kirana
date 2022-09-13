#include <iostream>

#include "app.hpp"

using std::cerr;
using std::cout;
using std::endl;


#ifdef COMPILE_BINDINGS
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

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
    m_windowManager.setOnAllWindowsClosedListener(
        [=]() { m_isRunning = false; });

    m_viewportWindow = m_windowManager.createWindow();
    m_viewport.init(m_viewportWindow);

    m_isRunning = true;
}

void kirana::Application::update()
{
    m_windowManager.update();
    m_viewport.update();
}

void kirana::Application::render()
{
    m_viewport.render();
}

void kirana::Application::clean()
{
    m_viewport.clean();
    m_windowManager.clean();
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
