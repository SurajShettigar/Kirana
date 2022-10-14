message("Generating Viewport library")

file(GLOB_RECURSE RENDERER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/*.cpp)
file(GLOB VIEWPORT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(viewport)
target_sources(viewport
        PUBLIC ${VIEWPORT_SOURCE_FILES}
        PRIVATE ${RENDERER_SOURCE_FILES})

target_include_directories(viewport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(viewport PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vulkan)
target_compile_definitions(viewport PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_link_libraries(viewport
        PUBLIC Vulkan::Vulkan vma-hpp
        PRIVATE scene window glm::glm math)
