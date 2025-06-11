## submodule vendoring with cmake

add_custom_target(vendor_submodules ALL)

macro(add_vendored_submodule name vendor)
find_package(Git QUIET REQUIRED)
set(VENDOR_SUBMODULE_${name} "vendor/${vendor}")
FetchContent_Declare(
    ${name}
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/${VENDOR_SUBMODULE_${name}}"
    DOWNLOAD_COMMAND ""
    ${ARGV}
    GIT_SHALLOW TRUE
    GIT_COMMAND ""
    SYSTEM OVERRIDE_FIND_PACKAGE
)
add_custom_target(vendor_submodule_update_${name}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${GIT_EXECUTABLE} submodule update --init ${VENDOR_SUBMODULE_${name}}
)
add_dependencies(vendor_submodules vendor_submodule_update_${name})
FetchContent_MakeAvailable(${name})
if(EVTLET_DEPS_QUIET)
set_directory_properties(FETCHCONTENT_SOURCE_DIR_$<UPPER_CASE:${name}> SYSTEM TRUE)
endif()
endmacro()

macro(assign_vendored_submodules)
# call this at the end of a file defining cmake targets,
# such that any target may depend on any vendored submodule
foreach(_TGT ${BUILDSYSTEM_TARGETS})
add_dependencies(vendor_submodules ${_TGT})
endforeach()
endmacro()
