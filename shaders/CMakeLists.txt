add_custom_target(Shaders)

file(MAKE_DIRECTORY ${SHADER_INSTALL_DIR})

function(buildShader SHADER_SOURCE_FILE)

    file(RELATIVE_PATH SHADER_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_SOURCE_FILE})
    get_filename_component(SHADER_NAME ${SHADER_RELATIVE_PATH} NAME)
    get_filename_component(SHADER_FOLDER ${SHADER_RELATIVE_PATH} DIRECTORY)

    file(GLOB_RECURSE SHADER_DEPENDENCIES
            ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FOLDER}/*.glsl
            ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FOLDER}/*.h)

    set(OUTPUT_PATH ${SHADER_INSTALL_DIR}/${SHADER_FOLDER}/${SHADER_NAME}.spv)

    add_custom_command(
            OUTPUT ${OUTPUT_PATH}
            COMMAND Vulkan::glslangValidator
            -V ${SHADER_SOURCE_FILE}
            -o ${OUTPUT_PATH}
            --target-env ${VULKAN_GLSLANG_VALIDATOR_TARGET}
            "$<$<CONFIG:Debug>:-g>"
            DEPENDS ${SHADER_SOURCE_FILE} ${SHADER_DEPENDENCIES}
            IMPLICIT_DEPENDS CXX ${SHADER_SOURCE_FILE})

    set_source_files_properties(${OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(Shaders PRIVATE ${OUTPUT_PATH})
endfunction()


file(GLOB_RECURSE SHADER_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/*.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/*.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rgen
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rmiss
        ${CMAKE_CURRENT_SOURCE_DIR}/*.rchit)

foreach (S ${SHADER_SOURCE_FILES})
    buildShader(${S})
endforeach ()