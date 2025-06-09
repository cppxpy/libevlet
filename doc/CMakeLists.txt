
set(DOC_BASEDIR ${CMAKE_BINARY_DIR}/doxygen/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOC_BASEDIR}/Doxyfile @ONLY)

add_custom_target(
    # this can be run under a normal cmake build
    # subsequently e.g ninja -C build 
    ${CMAKE_PROJECT_NAME}_doc ALL
    COMMENT "Builing documentation: ${CMAKE_PROJECT_NAME}"
    COMMAND doxygen ${DOC_BASEDIR}/Doxyfile
    WORKING_DIRECTORY ${DOC_BASEDIR}
)

# add_dependencies(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_doc)
