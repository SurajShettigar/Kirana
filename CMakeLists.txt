cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build using shared libraries" FORCE)

project(Kirana VERSION 0.1.0)

# Add external dependencies
find_package(Vulkan REQUIRED)
if(${Vulkan_FOUND})
    message("Found Vulkan: " ${Vulkan_VERSION})
endif()

# find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
# if(${Python_FOUND})
#     message("Found Python: " ${Python_VERSION})
# endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.76.0 EXACT COMPONENTS log REQUIRED)
if(${Boost_FOUND})
    message("Found Boost: " ${Boost_VERSION})
endif()

add_subdirectory(external)

# Add project files
add_subdirectory(kirana)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
