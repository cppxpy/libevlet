
include_directories(${PROJECT_SOURCE_DIR}/source)

include(Catch)

#
# interactive tests
#

add_executable(test_evt_cc "test_evt_cc.cpp")

#
# unit tests
#

add_executable(unit_test_evt_fiber "unit_test_evt_fiber.cpp")
target_link_libraries(unit_test_evt_fiber PRIVATE Catch2::Catch2WithMain)

# be sure to call include(CTest) before adding this dir
# in the parent CMakeLists.txt
catch_discover_tests(unit_test_evt_fiber)

#
# includes, linking
#

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
