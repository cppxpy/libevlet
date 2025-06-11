include(FetchContent)


## gcc/clang-specific
# add_compile_options("-Wno-error=float-equal")

#
# % libev
#

if(LIBEV_VENDOR_BUILD)
FetchContent_Declare(
    libev
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/libev
    DOWNLOAD_COMMAND ""
    GIT_REPOSITORY https://github.com/cppxpy/libev.git
    GIT_TAG v4.33-21-cmake0
    GIT_SHALLOW TRUE
    GIT_COMMAND ""
    SYSTEM OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(libev)
if(EVTLET_DEPS_QUIET)
set_directory_properties(FETCHCONTENT_SOURCE_DIR_LIBEV SYSTEM TRUE)
endif()
set(LIBEV_PC OFF PARENT_SCOPE)
else()
    include(FindPkgConfig)
    pkg_check_modules(libev REQUIRED libev>=4.0)
    set(LIBEV_PC ${libev_FOUND} PARENT_SCOPE)
endif() # LIBEV_VENDOR_BUILD

#
# % boost
#

#
#


set(BOOST_ASIO_COMPONENTS "system;coroutine;regex;thread"
    CACHE INTERNAL "library subset for BOOST_REQUIRED_COMPONENTS to be used with boost.asio")

set(BOOST_REQUIRED_COMPONENTS "context;fiber;${BOOST_ASIO_COMPONENTS}"
    CACHE INTERNAL "Required boost libraries (not header-only)")

find_package(Boost REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS} GLOBAL)
#
# % conditional/optional dependencies from the beman project
#


if(USE_BEMAN_OPTIONAL)
FetchContent_Declare(
    optional
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/optional
    DOWNLOAD_COMMAND ""
    GIT_REPOSITORY https://github.com/bemanproject/optional.git
    GIT_SHALLOW TRUE
    GIT_COMMAND ""
    SYSTEM OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(optional)
if(EVTLET_DEPS_QUIET)
set_directory_properties(FETCHCONTENT_SOURCE_DIR_OPTIONAL SYSTEM TRUE)
endif()
endif()

if(USE_BEMAN_SCOPE)
FetchContent_Declare(
    scope
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/scope
    DOWNLOAD_COMMAND ""
    GIT_REPOSITORY https://github.com/bemanproject/scope.git
    GIT_SHALLOW TRUE
    GIT_COMMAND ""
    SYSTEM OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(scope)
if(EVTLET_DEPS_QUIET)
set_directory_properties(FETCHCONTENT_SOURCE_DIR_SCOPE SYSTEM TRUE)
endif()
endif()
