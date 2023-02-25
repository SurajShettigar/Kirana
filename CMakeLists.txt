cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build using shared libraries" FORCE)
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
#if(${BUILD_SHARED_LIBS} STREQUAL ON)
#    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#endif()

project(Kirana VERSION 0.1.0)

# Add external dependencies
include(GNUInstallDirs)
find_package(Vulkan COMPONENTS glslangValidator REQUIRED)
if (${Vulkan_FOUND})
    message("Found Vulkan: " ${Vulkan_VERSION})
    string(REPLACE "." ";" Vulkan_VERSION_LIST ${Vulkan_VERSION})
    list(GET Vulkan_VERSION_LIST 0 Vulkan_VERSION_MAJOR)
    list(GET Vulkan_VERSION_LIST 1 Vulkan_VERSION_MINOR)
    set(VULKAN_GLSLANG_VALIDATOR_TARGET "vulkan${Vulkan_VERSION_MAJOR}.${Vulkan_VERSION_MINOR}")
    message("GLSL Validator Target: " ${VULKAN_GLSLANG_VALIDATOR_TARGET})
endif ()

# Setup python
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/venv/Scripts/python.exe")
    # Setup python virtual environment
    find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
    if (${Python_FOUND})
        message("Virtual Environment not found. Creating a virtual environment in current directory...")
        execute_process(
                COMMAND ${Python_EXECUTABLE} -m venv "${CMAKE_CURRENT_SOURCE_DIR}/venv"
                RESULT_VARIABLE VENV_RESULT
        )
        message("Python Virtual Environment Result: " ${VENV_RESULT})
        # Reset python to point to the new virtual environment
        unset(Python_EXECUTABLE)
    endif ()
endif ()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/venv/Scripts/python.exe")
    message("Python Virtual Environment Exists")
    set(Python_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/venv")
    find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
    if (${Python_FOUND})
        message("Found Python: " ${Python_VERSION})
        message("Python Location: " ${Python_EXECUTABLE})
        message("Python Library Location: " ${Python_SITELIB})
        # Setup python libraries
        message("Downloading required Python libraries...")
        execute_process(
                COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/venv/Scripts/pip" install -r "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
        )
    endif ()
else ()
    message(FATAL_ERROR "Failed to find Python virtual environment")
endif ()

add_subdirectory(external)

# Copy data directory
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
add_compile_definitions(DATA_DIR=\"${CMAKE_BINARY_DIR}/data\")

# Add shaders
set(SHADER_INSTALL_DIR ${CMAKE_BINARY_DIR}/shaders)
add_compile_definitions(SHADER_DIR=\"${SHADER_INSTALL_DIR}\")
add_subdirectory(shaders)

# Add project files
add_subdirectory(ui)
add_subdirectory(kirana)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
