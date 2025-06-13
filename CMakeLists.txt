
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
# % cache vars for project
#

set(BUILD_SHARED_LIBS ON CACHE BOOL
   "Build shared libraries")
set(BUILD_TESTS OFF CACHE BOOL
   "Build tests")

if(DEFINED ENV{BUILD_TESTS})
  message(STATUS "BUILD_TESTS defined in environment, enabling tests")
  set(BUILD_TESTS $ENV{BUILD_TESTS})
endif()

set(EVTLET_DOC OFF CACHE STRING
   "Build documentation for libevtlet")
set(EVTLET_DOC_ONLY ${EVTLET_DOC} CACHE STRING
   "Build only documentation for libevtlet" )

if(EVTLET_DOC)
  find_program(DOXYGEN_PROGRAM "doxygen" NO_CACHE)
  if(DOXYGEN_PROGRAM EQUAL "DOXYGEN_PROGRAM-notfound")
    message(WARNING "EVTLET_DOC was specified but doxygen was not found")
    message(WARNING "Not building libevtlet documentation")
    set(EVTLET_DOC OFF)
  endif()
endif()

set(EVTLET_DEPS_QUIET ON CACHE BOOL
   "Build libevtlet dependencies as boost SYSTEM dirs")

set(USE_BEMAN_OPTIONAL OFF CACHE BOOL
  "If enabled or stdlib <optionmal> is not available, use beman/optional")

set(USE_SCOPE_GUARD OFF CACHE BOOL
  "If enabled or stdlib <experimental/scope> is not available, use sr scope-guard")
set(SCOPEGUARD_UNITTEST OFF CACHE BOOL
  "Build unit tests for scope-guard")



# - Common Features
#

if(EVTLET_DOC)
add_subdirectory(doc)
endif()

if(NOT ${EVTLET_DOC_ONLY})

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

set(BUILD_SHARED_LIBS OFF CACHE BOOL
  "Build shared libraries")

## TBD
# set(EVT_LINK_SHARED ${BUILD_SHARED_LIBS} CACHE BOOL "Link libevtlet dependencies as shared libraries")

set(EVT_BUILD_EXAMPLES OFF CACHE BOOL
  "Build examples for libevtlet")

set(LIBEV_VENDOR_BUILD ON CACHE BOOL
  "Build and link with vendored libev sources")

#
# % vendored build options

#
# - ev
# - pybind11 version 3

#
# % vendored dependencies
#

include(CheckIncludeFile)

if(NOT ${USE_BEMAN_OPTIONAL})
  check_include_file("<optional>" HAVE_STD_OPTIONAL)
  if(NOT ${HAVE_STD_OPTIONAL})
    message(STATUS "stdlib <optional> not found, using beman/optional")
    set(USE_BEMAN_OPTIONAL TRUE)
  endif()
endif()

if(NOT ${USE_SCOPE_GUARD})
  check_include_file("<experimental/scope>" HAVE_STD_EXPERIMENTAL_SCOPE)
  if(NOT ${HAVE_STD_EXPERIMENTAL_SCOPE})
    message(STATUS "stdlib <experimental/scope> not found, using scope-guard")
    set(USE_SCOPE_GUARD ON CACHE BOOL "overriding value for USE_SCOPE_GUARD" FORCE)
  endif()
endif()

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)
include(cmake/vendoring.cmake)
add_subdirectory(vendor)

#
# % other dependencies
#

# fmt, spdlog
# boost for non-vendor build, handled in vendor/

#
# % project components
#

add_subdirectory(source)

#
# % tests
#

if(BUILD_TESTS)
  # preload for include(Catch)
  include(CTest)
  find_package(Catch2 QUIET)
  add_subdirectory(tests)
endif()


#
# % examples
#

if(${EVT_BUILD_EXAMPLES})
    add_subdirectory(example)
endif()

endif() ## EVTLET_DOC_ONLY
