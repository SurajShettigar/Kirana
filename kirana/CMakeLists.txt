add_subdirectory(utils)
add_subdirectory(window)
add_subdirectory(viewport)
add_subdirectory(scene)

add_executable(Kirana main.cpp app.cpp)

target_include_directories(Kirana PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Kirana PRIVATE utils window viewport)

# Generate python bindings
pybind11_add_module(Kirana_lib main.cpp app.cpp)
target_include_directories(Kirana_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Kirana_lib PRIVATE ${pybind11_INCLUDE_DIR})
target_link_libraries(Kirana_lib PRIVATE utils window viewport)
target_compile_definitions(Kirana_lib PRIVATE COMPILE_BINDINGS)
set_target_properties(Kirana_lib PROPERTIES
                                INTERPROCEDURAL_OPTIMIZATION ON
                                CXX_VISIBILITY_PRESET ON
                                VISIBILITY_INLINES_HIDDEN ON)
        