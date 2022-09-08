add_subdirectory(viewport)

file(GLOB_RECURSE KIRANA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(Kirana ${KIRANA_SOURCE_FILES})

target_include_directories(Kirana PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Kirana PRIVATE viewport)