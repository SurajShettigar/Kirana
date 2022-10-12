message("Generating Camera library")

file(GLOB_RECURSE CAMERA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(camera ${CAMERA_SOURCE_FILES})

target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(camera
        PUBLIC math
        PRIVATE utils)