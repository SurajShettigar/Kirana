message("Generating Scene library")

file(GLOB_RECURSE SCENE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(scene ${SCENE_SOURCE_FILES})

target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(scene PUBLIC math
        PRIVATE utils assimp stb)

add_executable(SceneTest scene_test.cpp)
target_link_libraries(SceneTest scene)