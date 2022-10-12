message("Generating Math library")

file(GLOB_RECURSE MATH_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(math ${MATH_SOURCE_FILES})

target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(math PRIVATE utils)

add_executable(MathTest math_test.cpp)
target_link_libraries(MathTest math)