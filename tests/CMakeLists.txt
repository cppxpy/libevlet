
include_directories(${PROJECT_SOURCE_DIR}/source)

include(Catch)

add_executable(test_skein_cond "test_skein_cond.cpp")
target_include_directories(test_skein_cond SYSTEM PRIVATE
    Boost::fiber
    "${PROJECT_SOURCE_DIR}/vendor/optional/include"
    "${PROJECT_SOURCE_DIR}/vendor/scope/include"
    )
target_link_libraries(test_skein_cond PRIVATE Boost::fiber)

if(USE_BEMAN_OPTIONAL)
    target_compile_definitions(test_skein_cond PRIVATE BEMAN_OPTIONAL)
endif()

if(USE_BEMAN_SCOPE)
    target_compile_definitions(test_skein_cond PRIVATE BEMAN_SCOPE)
endif()

assign_vendored_submodules()
