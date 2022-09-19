add_subdirectory(utils)
add_subdirectory(window)
add_subdirectory(viewport)

add_executable(Kirana main.cpp app.cpp)

target_include_directories(Kirana PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Kirana PRIVATE utils window viewport)

# Generate python bindings
pybind11_add_module(kirana_app main.cpp app.cpp)
target_include_directories(kirana_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(kirana_app PRIVATE ${pybind11_INCLUDE_DIR})
target_link_libraries(kirana_app PRIVATE utils window viewport)
target_compile_definitions(kirana_app PRIVATE COMPILE_BINDINGS)
set_target_properties(kirana_app PROPERTIES
                                INTERPROCEDURAL_OPTIMIZATION ON
                                CXX_VISIBILITY_PRESET ON
                                VISIBILITY_INLINES_HIDDEN ON)
        