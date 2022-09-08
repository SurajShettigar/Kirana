cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Kirana VERSION 0.1.0)

# Add external dependencies
find_package(Vulkan REQUIRED)
message("Found Vulkan: " ${Vulkan_FOUND})
add_subdirectory(external)

# Add project files
add_subdirectory(kirana)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
