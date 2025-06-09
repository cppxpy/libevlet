
cmake_minimum_required(VERSION 3.14)

project(
    libevtlet
    VERSION 0.1.1
    DESCRIPTION "Integrating boost.fiber with libev, libuv, and boost asio"
    HOMEPAGE_URL "https://github.com/evtlet/libevtlet"
    LANGUAGES CXX
)

# C support for find_package
enable_language(C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

#
# % cmake vars for project
#

set(EVTLET_DOC OFF CACHE STRING "build documentation for libevtlet")
set(EVTLET_DOC_ONLY ${EVTLET_DOC} CACHE STRING "build only documentation for libevtlet" )

if(EVTLET_DOC)
  find_program(DOXYGEN_PROGRAM "doxygen" NO_CACHE)
  if(DOXYGEN_PROGRAM EQUAL "DOXYGEN_PROGRAM-notfound")
    message(WARNING "EVTLET_DOC was specified but doxygen was not found")
    message(WARNING "Not building libevtlet documentation")
    set(EVTLET_DOC OFF)
  endif()
endif()


#
# - Common Features
#

if(EVTLET_DOC)
add_subdirectory(doc)
endif()

if(NOT EVTLET_DOC_ONLY)

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

set(EVT_BUILD_EXAMPLES OFF CACHE BOOL "Build examples for libevtlet")

set(LIBEV_VENDOR_BUILD ON CACHE BOOL "Build vendored libev sources")


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

endif() ## EVTLET_DOC_ONLY
