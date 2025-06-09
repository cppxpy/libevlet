include(FetchContent)

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)

## gcc/clang-specific
# add_compile_options("-Wno-error=float-equal")

#
# % libev
#

if(EVT_VENDOR_LIBEV)
FetchContent_Declare(
    libev
    SYSTEM
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/libev
    DOWNLOAD_COMMAND git submodule update --init vendor/libev
    GIT_REPOSITORY https://github.com/cppxpy/libev.git
    GIT_TAG v4.33-21-cmake0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libev)
set(LIBEV_PC OFF PARENT_SCOPE)
else()
include(FindPkgConfig)
pkg_check_modules(libev REQUIRED libev>=4.0)
set(LIBEV_PC ON PARENT_SCOPE)
endif() # EVT_VENDOR_LIBEV

#
# % boost
#

#
# % fmt
#

#
# % spdlog 2.x
#
