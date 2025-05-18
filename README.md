# libevtlet : Integrating boost.fiber with libev, libuv, and boost asio

inspired by greenlet and Gevent modules for Python platforms.

## Build System

This project uses [CMake][cmake]

### Building with Presets

Inspired by [`friendlyanon/cmake-init`][cmake-init], this project
uses [CMake presets][cmake-presets].

**Presets for CMake Build Type**

The following intermediate presets are defined, such as to be
inherited when defining user-visible presets for individual
host platforms:
- For debug builds: `dev-base`
- For release builds with debug info: `release`
- For release builds without debug info: `release2`


**Platform-specific presets**

Similarly, the following presets are defined for use when
creating a preset for a single host platform:

- `vs2017` for hosts running Microsoft Windows, assuming
  the Microsoft Visual C (msvc) tools are installed
- `ninja-osx` for hosts running OS X
- `ninja-main` for hosts not running OS X and not building
  with msvc. When used with other presets, this may be
  abbreviated as `ninja`

**Concrete Presets**

This results in a set of nine user-facing presets for build, e.g:
- `ninja-dev` for development builds on Linux, BSD, and
  other UNIX-like platforms
- `vs2017-release` for release builds with debug info, on msvc platforms
- `osx-release2` for release builds without debug info, on xcode platforms

Each of these presets may be selected using CMake support for
IDE platforms including Visual Studio Code, and may be selected
directly at the command line.

```bash
cmake --preset ninja-dev -B build && ninja -C build/dev
```

These presets may also be extended with user-defined presets,
in `CMakeUserPresets.json`.

#### User-Defined Presets

Fach of these presets will be available for extension in the file
`CMakeUserPresets.json`.

**Example: `CMakeUserPresets.json`**
```json
{
  "version": 2,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 14,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "dev",
      "inherits": [
        "ninja-dev"
      ],
      "cacheVariables": {
        "CMAKE_EXPORT_COMPILE_COMMANDS": true,
        "FETCHCONTENT_FULLY_DISCONNECTED": true,
        "BUILD_SHARED_LIBS": false,
        "EVT_VENDOR_LIBEV": true,
        "CMAKE_FIND_ROOT_PATH": "/opt/miniforge3/envs/libevtlet;/usr"
      },
      "environment": {
        "PKG_CONFIG_PATH": "/opt/miniforge3/envs/libevtlet/lib/pkgconfig:/usr/lib64/pkgconfig",
        "CMAKE_BUILD_PARALLEL_LEVEL": 8
      }
    }
  ],
  "buildPresets": [
    {
      "name": "dev",
      "configurePreset": "dev",
      "configuration": "Debug",
      "jobs": 8
    }
  ],
  "testPresets": [
    {
      "name": "dev",
      "configurePreset": "dev",
      "configuration": "Debug",
      "output": {
        "outputOnFailure": true
      },
      "execution": {
        "jobs": 8,
        "noTestsAction": "error"
      }
    }
  ]
}
```

Features of the `dev` preset defined in this example:
- Development build
- Parallel build with up to 8 workers
- Using ninja, with compiler flags common to GCC and clang compilers
- Using compiler and dependencies installed to the project virtual
  env, given the value for `CMAKE_FIND_ROOT_PATH`
- Generating `compiler_commands.json`, for IDE tools and other
  static analysis
- Not updating vendored sources under CMake fetch content
- Building and linking with static libraries
- Building the vendored libev sources, using configuraition provided
  in this project

This examples assumes that a python virtual environment has been
installed. Some documentation for this procedure is provided below.

## Installation

### Vendored Dependencies

Vendored dependencies with git submodules:
- libev
- fmt
- spdlog
- boost

**Known Limitation: Compatibility for Vendored Builds**

It's assumed that the vendored builds may not be fully compatible
with other builds for the same source projects.

At the time of this writing, the vendored sources have been used
only for purpose of development. The vendored dependencies should
generally not be installed, except in any of the following cases:

- When this project can be assumed to provide the primary version
  of each vendored component, i.e providing the host component
  from the vendored sources for this project.

- Ostensibly, when components provided by this project will be
  produced only with static linking for these vendored components.
  (Need Test)

- After testing for dynamic linking with components using the same
  libraries as produced under vendored builds in this project.

It's hoped that the concerns about compatibility might be mitigated
with static linking for components produced in this project. This
needs further testing.

**Known Limitation: spdlog version 2 availability

It should generally be possible to use the host's alternative for every
vendored component, except in the case of the host's spdlog build. Generally,
the host's spdlog build can be assumed to have been produced from spdlog
version 1 sources.

In the interest of providing support for spdlog version 2 in release builds,
the libevtlet project should provide testing for ABI compatibility across
spdlog builds from version 1 and version 2 sources.

### Virtual Environment

#### Toolchain and Build Dependencies via Virtual Environment

A subset of project depenencies are available via [miniforge3][miniforge3]
and [conda][conda]. These dependencies would include:
- Compiler toolchains (GCC, clang)
- Linting tools (cppcheck, clang-tidy)
- boost libraries
- [libev][libev]
- [libuv][libuv]
- [Catch2][catch2-src]


For platforms where [miniforge3][miniforge3] or [conda][conda] would be
avaialble, these dependencies can be installed using `./environment.yml`

To create an environment named `libevtlet`, with this project's source
directory selected as the current working directory:

```bash
$ conda env create --file ./environment.yml
```

After this virtual environment has been created, any shell tools from
the environment can be run from a command shell using the local utility
script `./mini_shell.sh`

```bash
$ ./mini_shell.sh ipython
```

**Integrating the Virtual Environment with CMake**

When the appropriate virtual environment path is specified in
`CMAKE_FIND_ROOT_PATH`, then CMake should be able to locate
any dependencies installed under the virtual environment.

For locating any dependencies selected via pkgconfig,
it may also be helpful to update the pkgconfig path when
running the CMake command.


**Example: Run CMake with dependencies from virtual environment**

```bash
$ VENV_ROOT=$(./mini_shell.sh conda env list |
    awk '$1 == "libevtlet" { print $NF ; exit; }')
$ env PKG_CONFIG_PATH="${VENV_ROOT}/lib/pkgconfig:/usr/lib64/pkgconfig"  \
    cmake -DCMAKE_FIND_ROOT_PATH="${VENV_ROOT};/usr" \
          --preset ninja-dev && ninja -C build/dev
```

On other platforms, these dependencies should be installed
using the host package management system.

#### pybind11 and the Virtual Environment

Once a stable API is in place, this project should provide a Python
interface via [pybind11]. As a build dependency, this would also be
available via [conda] and [miniforge3].

### Vendored Dependencies

- libev
- boost
- fmt, spdlog


## Reference Documentation

- [boost.fiber][boost.fiber]
- [boost.context][boost.context]
- [Introduction to boost.fiber][fiber-notes] by Andy Dunstall
- Event Frameworks
  - [libev][libev]
  - [libuv][libuv]
  - [boost.asio][boost.asio]
- Support Libraries
  - [boost.intrusive][boost.intrusive]
  - [spdlog v2.x][spdlog2]
- Python APIs
  - [greenlet][greenlet]
  - [gevent][gevent]
- Embedded Python
  - [Python/C API Reference Manual][cpython-ref]
  - [PEP 523: Adding a frame evaluation API to CPython][pep-523]
  - [PEP 741: C API to configure Python initialization][pep-741]
  - [pybind11][pybind11]
- Misc
  - Concerning `clock_gettime` usage and syscalls, such as availble with [libev][libev]
    - [Bert Hubert: On Linux vDSO and clock_gettime sometimes being slow](https://berthub.eu/articles/posts/on-linux-vdso-and-clockgettime/)

## Project Dependencies

version sync for boost: [boost libraries][boost-lib]

- [boost.fiber][boost.fiber-src]
- Conditional Runtime Dependencies
  - [libev][libev-src]
  - [uvw][uvw], [libuv][libuv-src]
  - [boost.asio][boost.asio]
- Other Runtime Dependencies
  - [fmt][fmt-src]
  - [spdlog v2.x][spdlog2]
- Dependencies for Tests
  - [Catch2][catch2-src]


[boost-lib]: https://github.com/boostorg/boost/tree/master/libs
[boost.fiber]: https://live.boost.org/doc/libs/1_88_0/libs/fiber/doc/html/index.html
[boost.fiber-src]: https://github.com/boostorg/fiber
[boost.context]: https://live.boost.org/doc/libs/1_88_0/libs/context/doc/html/index.html
[boost.asio]: https://www.boost.org/doc/libs/1_88_0/doc/html/boost_asio.html
[boost.intrusive]: https://www.boost.org/doc/libs/1_88_0/doc/html/intrusive.html
[libev]: https://metacpan.org/dist/EV/view/libev/ev.pod
[libev-src]: https://software.schmorp.de/pkg/libev.html
[libuv]: https://libuv.org/
[libuv-src]: https://github.com/libuv/libuv
[uvw]: https://github.com/skypjack/uvw
[fmt-src]: https://github.com/fmtlib/fmt/
[spdlog2]: https://github.com/gabime/spdlog/tree/v2.x
[greenlet]: https://greenlet.readthedocs.io/en/latest/
[gevent]: https://www.gevent.org/
[catch2-src]: https://github.com/catchorg/Catch2/
[fiber-notes]: https://andydunstall.medium.com/boost-fibers-7c262bb3057d
[cpython-ref]: https://docs.python.org/3/c-api/index.html
[pep-523]: https://peps.python.org/pep-0523/
[pep-741]: https://vstinner.github.io/pyconfig-pep-741.html
[pybind11]: https://pybind11.readthedocs.io/
[miniforge3]: https://github.com/conda-forge/miniforge
[conda]: https://conda.io/
[cmake]: https://cmake.org/
[cmake-init]: https://github.com/friendlyanon/cmake-init/
[cmake-presets]: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
