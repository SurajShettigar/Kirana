message("Generating Viewport library")

file(GLOB RENDERER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp)
file(GLOB VIEWPORT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(viewport)
target_sources(viewport PUBLIC ${VIEWPORT_SOURCE_FILES}
                        PRIVATE ${RENDERER_SOURCE_FILES})       

target_include_directories(viewport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(viewport PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/renderer)
target_link_libraries(viewport Vulkan::Vulkan window)