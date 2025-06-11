# sandbox

set(BUILD_OFFSITE_EXAMPLES ON CACHE BOOL "build examples from off-site projects")
if(BUILD_OFFSITE_EXAMPLES)
#
# examples adpated from other projects
#

add_executable(
    ev_example
    ev_example.cpp
)
target_link_libraries(ev_example ev)


add_executable(
    fiber_simple
    fiber_simple.cpp
)
target_link_libraries(fiber_simple PRIVATE Boost::fiber)

endif() # if(BUILD_OFFSITE_EXAMPLES


set(evtlet_targets_ev ${evtlet_targets_ev} PARENT_SCOPE)
