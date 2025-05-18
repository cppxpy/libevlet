
cmake_minimum_required(VERSION 3.14)

project(
    libevtlet
    VERSION 0.1.1
    DESCRIPTION "Integrating boost.fiber with libev, libuv, and boost asio"
    HOMEPAGE_URL "https://github.com/cppxpy/libevtlet"
    LANGUAGES CXX
)

# C support for find_package
enable_language(C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# include_directories(SYSTEM vendor/include)
# include_directories(${CMAKE_CURRENT_BINARY_DIR}/gen_include)

#
# % debug
#

# if(DEFINED CMAKE_FIND_ROOT_PATH)
# message(STATUS "CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH}")
# else()
# message(STATUS "CMAKE_FIND_ROOT_PATH: (Using default)")
# endif()

#
# % project build options
#

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

set(EVT_LINK_SHARED ${BUILD_SHARED_LIBS} CACHE BOOL "Link libevtlet dependencies as shared libraries")

if(${EVT_LINK_SHARED})
set(EVT_DEP_VISIBILITY PUBLIC)
else()
set(EVT_DEP_VISIBILITY PRIVATE)
endif()

set(EVT_BUILD_EXAMPLES OFF CACHE BOOL "Build examples for libevtlet")

set(EVT_VENDOR_LIBEV  ON CACHE BOOL "Build vendored libev sources")


#
# % vendored build options
#

# - ev
# - fmt, spdlog

#
# % vendored dependencies
#

add_subdirectory(vendor)

set(evtlet_targets_ev "" CACHE INTERNAL "targets that must be linked to libev")

#
# % project components
#

add_subdirectory(source)

#
# % tests
#

add_subdirectory(tests)


#
# % examples
#

if(${EVT_BUILD_EXAMPLES})
    add_subdirectory(example)
endif()


# message(STATUS "ev tgts: ${evtlet_targets_ev}")

if(${LIBEV_PC})
foreach(tgt IN LISTS evtlet_targets_ev)
    # DNW No targets will have been defined when this file is processed
    foreach(lib IN LISTS libev_LIBRARIES)
        target_link_libraries(${tgt} ${EVT_DEP_VISIBILITY} ${lib})
    endforeach()
    foreach(dir IN LISTS libev_LIBRARY_DIRS)
        target_link_directories(${tgt} ${EVT_DEP_VISIBILITY} ${dir})
    endforeach()
    foreach(dir IN LISTS libev_INCLUDE_DIRS)
        target_include_directories(${tgt} ${EVT_DEP_VISIBILITY} ${dir})
    endforeach()
    foreach(flag IN LISTS libev_CFLAGS)
        target_compile_definitions(${tgt} ${EVT_DEP_VISIBILITY} ${flag})
    endforeach()
endforeach()
else()
foreach(tgt IN LISTS evtlet_targets_ev)
    target_link_libraries(${tgt} ${EVT_DEP_VISIBILITY} ev)
endforeach()
endif()
