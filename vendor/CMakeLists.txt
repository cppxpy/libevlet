include(FetchContent)

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)

## gcc/clang-specific
# add_compile_options("-Wno-error=float-equal")

#
# % libev
#

if(${LIBEV_VENDOR_BUILD})
FetchContent_Declare(
    libev
    SYSTEM
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/libev
    DOWNLOAD_COMMAND ""
    GIT_REPOSITORY https://github.com/cppxpy/libev.git
    GIT_TAG v4.33-21-cmake0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libev)
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
