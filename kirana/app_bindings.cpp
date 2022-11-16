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
    py::class_<kirana::Application>(m, "Application")
        .def(py::init())
        .def("run", static_cast<void (kirana::Application::*)(
                        long, int, int)>(&kirana::Application::run));
}
#endif