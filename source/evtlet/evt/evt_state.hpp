
#pragma once

#include <cstddef>

namespace evtlet {

enum class evt_state : size_t {
  STATE_NONE = 0,
  STATE_PENDING = 1,
  STATE_DETACHED = 2,
  STATE_DONE = 4,
  // STATE_CANCEL = 8,
  STATE_PENDING_DETACHED = STATE_PENDING | STATE_DETACHED
};

} // namespace evtlet
