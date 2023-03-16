add_subdirectory(utils)
add_subdirectory(math)
add_subdirectory(window)
add_subdirectory(viewport)
add_subdirectory(new_scene)

add_executable(Kirana main.cpp app.cpp)

target_include_directories(Kirana PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Kirana PRIVATE utils window scene viewport)
add_dependencies(Kirana Shaders)
        