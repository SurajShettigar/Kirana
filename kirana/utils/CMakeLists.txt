message("Generating Utils library")

file(GLOB_RECURSE UTILS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(utils ${UTILS_SOURCE_FILES})

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if (${BUILD_SHARED_LIBS} STREQUAL ON)
    target_compile_definitions(utils PUBLIC BOOST_ALL_DYN_LINK)
endif ()
target_link_libraries(utils
        PUBLIC Boost::log math)