# Add pybind11 dependency
message("<----Adding PYBIND11---->")
add_subdirectory(pybind11)

# Add GLFW dependency
message("<----Adding GLFW---->")
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# Add ASSIMP dependency
message("<----Adding ASSIMP---->")
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
# Set necessary importers
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_PLY_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(assimp)

# Add Vulkan Memory Allocator dependency
message("<----Adding VMA-HPP---->")
add_library(vma-hpp INTERFACE)
target_include_directories(vma-hpp INTERFACE vma-hpp)

# Add SPDLOG
message("<----Adding SPDLOG---->")
add_subdirectory(spdlog)

# Add LZ4
message("<----Adding LZ4---->")
set(LZ4_BUNDLED_MODE ON)
add_subdirectory(lz4/build/cmake)

# Add STB
message("<----Adding STB---->")
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)