
include_directories(${PROJECT_SOURCE_DIR}/source)

include(Catch)

add_executable(test_evt_fiber_cond "test_evt_fiber_cond.cpp")

add_executable(test_evt_cc "test_evt_cc.cpp")


get_directory_property(_DIR_TARGETS BUILDSYSTEM_TARGETS)

foreach(_TGT ${_DIR_TARGETS})
  target_include_directories(${_TGT} SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS}
  )
  target_link_libraries(${_TGT} PRIVATE Boost::fiber)

  if(USE_BEMAN_OPTIONAL)
    target_include_directories(${_TGT} PRIVATE ${optional_INCLUDE_DIRS})
    target_compile_definitions(${_TGT} PRIVATE USE_BEMAN_OPTIONAL)
  endif()

  if(USE_SCOPE_GUARD)
    target_include_directories(${_TGT} PRIVATE ${scope_guard_INCLUDE_DIRS})  
    target_compile_definitions(${_TGT} PRIVATE USE_SCOPE_GUARD)
  endif()
endforeach()


assign_vendored_submodules()
