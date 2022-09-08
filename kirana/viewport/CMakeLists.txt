message("Generating Viewport library")

file(GLOB_RECURSE VIEWPORT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(viewport ${VIEWPORT_SOURCE_FILES})

target_include_directories(viewport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(viewport PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(viewport Vulkan::Vulkan)