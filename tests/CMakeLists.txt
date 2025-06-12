
include_directories(${PROJECT_SOURCE_DIR}/source)

include(Catch)

add_executable(test_evt_fiber_cond "test_evt_fiber_cond.cpp")

#
# non-conditional incluees, libraries
#
target_include_directories(test_evt_fiber_cond SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS}
  )
target_link_libraries(test_evt_fiber_cond PRIVATE Boost::fiber)

#
# conditional includes
#
if(USE_BEMAN_OPTIONAL)
  target_include_directories(test_evt_fiber_cond PRIVATE ${optional_INCLUDE_DIRS})
  target_compile_definitions(test_evt_fiber_cond PRIVATE USE_BEMAN_OPTIONAL)
endif()

if(USE_SCOPE_GUARD)
  target_include_directories(test_evt_fiber_cond PRIVATE ${scope_guard_INCLUDE_DIRS})  
  target_compile_definitions(test_evt_fiber_cond PRIVATE USE_SCOPE_GUARD)
endif()

assign_vendored_submodules()
