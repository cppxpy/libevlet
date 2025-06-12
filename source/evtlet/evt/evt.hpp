#pragma once

#include <cstddef>

namespace evtlet {

template <typename T> class evt {

public:
  evt() = default;
  virtual ~evt() = default;

  /// not copyable
  evt(evt const &) = delete;

  /// not assignable
  evt &operator=(evt const &) = delete;

  virtual void wait() = 0;
  virtual T &get() = 0;
  virtual bool done() noexcept = 0;

};

} // namespace evtlet