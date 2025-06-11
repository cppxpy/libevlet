
include_directories(${PROJECT_SOURCE_DIR}/source)

add_library(evtlet
    "evt.cpp"
)

assign_vendored_submodules()

