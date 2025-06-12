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
set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} PARENT_SCOPE)

if(USE_BEMAN_OPTIONAL)
  add_vendored_submodule(optional optional
    GIT_REPOSITORY https://github.com/bemanproject/optional.git
  )
  set(optional_INCLUDE_DIRS ${beman_optional_SOURCE_DIR}/include PARENT_SCOPE)
endif()

if(USE_SCOPE_GUARD)
  add_vendored_submodule(scope_guard scope-guard
    GIT_REPOSITORY https://github.com/offa/scope-guard.git
  )
  set(scope_guard_INCLUDE_DIRS ${ScopeGuard_SOURCE_DIR}/include PARENT_SCOPE)
endif()
