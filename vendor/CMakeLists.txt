include(FetchContent)

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
    # GIT_REPOSITORY https://github.com/gabime/spdlog.git
    # GIT_TAG rel-4_33
    # GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libev)
else()
# assume libev.pc is available on the host
# - not available with the present feedstock build
# - would probably need to be adjusted at conda pkg install time
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
