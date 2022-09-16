message("Generating Utils library")

# file(GLOB_RECURSE UTILS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# add_library(utils ${UTILS_SOURCE_FILES})
add_library(utils INTERFACE)

target_include_directories(utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utils INTERFACE Boost::log)