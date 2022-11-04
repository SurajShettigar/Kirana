# Generate python bindings
pybind11_add_module(KiranaPython ../kirana/app.cpp)
target_include_directories(KiranaPython PRIVATE ../kirana)
target_include_directories(KiranaPython PRIVATE ${pybind11_INCLUDE_DIR})
target_link_libraries(KiranaPython PRIVATE utils window scene viewport)
target_compile_definitions(KiranaPython PRIVATE COMPILE_BINDINGS
        PY_MODULE_NAME=KiranaPython)
set_target_properties(KiranaPython PROPERTIES
        CXX_VISIBILITY_PRESET ON
        VISIBILITY_INLINES_HIDDEN ON
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})
install(TARGETS KiranaPython
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Compile python file into an executable
set(INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app.py)
set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(OUTPUT_PATH ${OUTPUT_DIR}/Kirana.exe)
get_filename_component(PYINSTALLER ${Python_EXECUTABLE} DIRECTORY)
set(PYINSTALLER ${PYINSTALLER}/pyinstaller)
add_custom_command(
        TARGET KiranaPython POST_BUILD
        COMMAND ${PYINSTALLER} -p ${CMAKE_CURRENT_BINARY_DIR} -F ${INPUT_PATH} -n Kirana --distpath ${OUTPUT_DIR} --workpath ${OUTPUT_DIR}
        BYPRODUCTS ${OUTPUT_PATH}
        VERBATIM)

add_dependencies(KiranaPython Shaders)
