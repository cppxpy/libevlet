

add_library(evtlet
    "evt.cpp"
)

list(APPEND evtlet_targets_ev evtlet)


set(evtlet_targets_ev ${evtlet_targets_ev} PARENT_SCOPE)