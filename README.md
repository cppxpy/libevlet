# libevtlet : Integrating boost.fiber with libev, libuv, and boost asio

inspired by greenlet and Gevent modules for Python platforms.

## Build System

CMake

## Documentation

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

## Project Dependencies

version sync for boost: [boost libraries][boost-lib]

- [boost.fiber][boost.fiber-src]
- Conditional Dependencies
  - [libev][libev-src]
  - [uvw][uvw], [libuv][libuv-src]
  - [boost.asio][boost.asio]
- Other Runtime Dependencies
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
[spdlog2]: https://github.com/gabime/spdlog/tree/v2.x
[greenlet]: https://greenlet.readthedocs.io/en/latest/
[gevent]: https://www.gevent.org/
[catch2-src]: https://github.com/catchorg/Catch2/
[fiber-notes]: https://andydunstall.medium.com/boost-fibers-7c262bb3057d
