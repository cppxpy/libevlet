# examples, adpated from other projects

add_executable(
    ev_example
    ev_example.cpp
)

list(APPEND evtlet_targets_ev ev_example)


add_executable(
    fiber_simple
    fiber_simple.cpp
)
target_link_libraries(fiber_simple PRIVATE Boost::fiber)


set(evtlet_targets_ev ${evtlet_targets_ev} PARENT_SCOPE)
