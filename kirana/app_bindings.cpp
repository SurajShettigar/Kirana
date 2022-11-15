#include "app.hpp"

#ifdef COMPILE_BINDINGS
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif


/**
 * @brief Construct Python Binding for Application
 *
 */
#ifdef COMPILE_BINDINGS
PYBIND11_MODULE(PY_MODULE_NAME, m)
{
    py::class_<kirana::window::Window, std::shared_ptr<kirana::window::Window>>(
        m, "Window")
        .def_static("onWindowResize",
                    &kirana::window::Window::onWindowResizeBindFunc)
        .def_static("onWindowClose", &kirana::window::Window::onWindowCloseBindFunc)
        .def("__repr__", [](const kirana::window::Window &w) {
            return "<kirana.window.Window>";
        });

    py::class_<kirana::window::PlatformWindow, kirana::window::Window,
               std::shared_ptr<kirana::window::PlatformWindow>>(
        m, "PlatformWindow")
        .def("__repr__", [](const kirana::window::PlatformWindow &w) {
            return "<kirana.window.PlatformWindow>";
        });

    py::class_<kirana::Application>(m, "Application")
        .def(py::init())
        .def("run", static_cast<void (kirana::Application::*)(
                        long, int, int)>(&kirana::Application::run))
        .def("getViewportWindow", &kirana::Application::getViewportWindow);
}
#endif