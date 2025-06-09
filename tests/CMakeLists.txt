
include_directories(${PROJECT_SOURCE_DIR}/source)

find_package(Catch2 REQUIRED)
include(Catch)

add_executable(test_skein_cond "test_skein_cond.cpp")
target_include_directories(test_skein_cond SYSTEM PRIVATE Boost::fiber)
target_link_libraries(test_skein_cond PRIVATE Boost::fiber)

