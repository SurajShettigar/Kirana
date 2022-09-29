cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build using shared libraries" FORCE)
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
#if(${BUILD_SHARED_LIBS} STREQUAL ON)
#    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#endif()

project(Kirana VERSION 0.1.0)

# Add external dependencies
find_package(Vulkan COMPONENTS glslangValidator REQUIRED)
if (${Vulkan_FOUND})
    message("Found Vulkan: " ${Vulkan_VERSION})
endif ()

# find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
# if(${Python_FOUND})
#     message("Found Python: " ${Python_VERSION})
# endif()

if (${BUILD_SHARED_LIBS} STREQUAL ON)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_DEBUG_LIBS OFF)
    set(Boost_USE_RELEASE_LIBS ON)
else ()
    set(Boost_USE_STATIC_LIBS ON)
endif ()
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.76.0 EXACT COMPONENTS log REQUIRED)
if (${Boost_FOUND})
    message("Found Boost: " ${Boost_VERSION})
endif ()

add_subdirectory(external)

# Copy data directory
file(COPY data DESTINATION  ${CMAKE_BINARY_DIR})
add_compile_definitions(DATA_DIR=\"${CMAKE_BINARY_DIR}/data\")

# Add shaders
set(SHADER_INSTALL_DIR ${CMAKE_BINARY_DIR}/shaders)
add_compile_definitions(SHADER_DIR=\"${SHADER_INSTALL_DIR}\")
add_subdirectory(shaders)

# Add project files
add_subdirectory(kirana)
add_dependencies(Kirana Shaders)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
