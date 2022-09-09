message("Generating Window library")

file(GLOB_RECURSE WINDOW_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(window ${WINDOW_SOURCE_FILES})

target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(window glfw)