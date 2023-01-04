add_custom_target(Shaders)

file(MAKE_DIRECTORY ${SHADER_INSTALL_DIR})

function(buildShader SHADER_SOURCE_FILE)
    get_filename_component(SHADER_NAME ${SHADER_SOURCE_FILE} NAME)
    set(INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME})
    set(OUTPUT_PATH ${SHADER_INSTALL_DIR}/${SHADER_NAME}.spv)
    add_custom_command(
            OUTPUT ${OUTPUT_PATH}
            COMMAND Vulkan::glslangValidator -V ${INPUT_PATH} -o ${OUTPUT_PATH} --target-env ${VULKAN_GLSLANG_VALIDATOR_TARGET}
            DEPENDS ${INPUT_PATH}
            IMPLICIT_DEPENDS CXX ${INPUT_PATH}
            VERBATIM)
    set_source_files_properties(${OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(Shaders PRIVATE ${OUTPUT_PATH})
endfunction()


file(GLOB SHADER_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/*.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/*.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rgen
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rmiss
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rchit)

foreach (S ${SHADER_SOURCE_FILES})
    buildShader(${S})
endforeach ()