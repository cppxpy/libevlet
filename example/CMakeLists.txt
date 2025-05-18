# examples, adpated from other projects

add_executable(
    ev_example
    ev_example.cpp
)

target_link_libraries(ev_example PRIVATE ev)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()


find_package(Boost REQUIRED COMPONENTS fiber)

add_executable(
    fiber_simple
    fiber_simple.cpp
)
target_link_libraries(fiber_simple Boost::fiber)
