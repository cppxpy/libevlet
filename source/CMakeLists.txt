

add_library(evtlet
    "evt.cpp"
)

target_link_libraries(evtlet ${EVT_DEP_VISIBILITY} ev)
