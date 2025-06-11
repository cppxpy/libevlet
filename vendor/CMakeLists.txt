## vendored submodules for libevtlet

include(FetchContent)


## gcc/clang-specific
# add_compile_options("-Wno-error=float-equal")

#
# % libev
#

if(LIBEV_VENDOR_BUILD)
  add_vendored_submodule(libev libev
    GIT_REPOSITORY https://github.com/cppxpy/libev.git
    GIT_TAG v4.33-21-cmake0)
  set(LIBEV_PC OFF PARENT_SCOPE)
else()
  include(FindPkgConfig)
  pkg_check_modules(libev REQUIRED libev>=4.0)
  set(LIBEV_PC ${libev_FOUND} PARENT_SCOPE)
endif() # LIBEV_VENDOR_BUILD

#
# % boost
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
add_vendored_submodule(optional optional
  GIT_REPOSITORY https://github.com/bemanproject/optional.git
)
endif()

if(USE_BEMAN_SCOPE)
add_vendored_submodule(scope scope
  GIT_REPOSITORY https://github.com/bemanproject/scope.git
)
endif()
