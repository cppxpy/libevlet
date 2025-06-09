/**
 * @file skein_cond.hpp
 * @brief prototype (with condition variables) onto boost.fiber and
 * boost.context
 *
 * @copyright Copyright (c) 2025 Sean Champ
 *
 */

// cf. boost libs/fiber/examples/future.cpp

#pragma once

#include <memory>
#include <mutex>

#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/future/async.hpp>
#include <boost/fiber/future/future.hpp>
#include <boost/fiber/mutex.hpp>

namespace evtlet {

enum class skein_state : size_t {
  STATE_PENDING = 0,
  STATE_DONE = 1,
  STATE_ACQUIRED = 2
};

template <typename T> class skein {
  /**
   * @brief prototype for an API onto a boost fiber future
   *
   */

public:
  using value_type = T;
  using ptr_type = std::unique_ptr<value_type>;
  using future_type = boost::fibers::future<ptr_type>;
  using value_lock_type = boost::fibers::mutex;
  using condition_lock_type = boost::fibers::mutex;
  using condition_type = boost::fibers::condition_variable;

  virtual ~skein() = default;

  // a new fiber will be created and detached
  // from the call to async() in the ctor here

  skein()
      : m_future(std::move(
            boost::fibers::async(std::bind(&skein<value_type>::func_p, this)))),
        m_state(skein_state::STATE_PENDING) {};

  /// no copy
  skein(skein const &) = delete;

  /// no assign
  skein &operator=(skein const &) = delete;

  virtual T *get() {
    {
      {
        /// acquire m_cv_lock, test state and update
        /// value if completed
        std::unique_lock<condition_lock_type> cv_lck(m_cv_lock);
        while (!static_cast<bool>(get_state())) {
          m_cond.wait(cv_lck);
        }
      }
      if (m_future.valid()) {
        /// this fiber will retrieve the future value
        std::scoped_lock<value_lock_type> lck(m_lock);
        /// Implementation note: future state
        ///
        /// When not using a shared future, the future
        /// will no longer be valid() after this call.
        ///
        /// This state condition is assumed also in
        /// the implementation of this->get_state()
        ///
        /// FIXME test with a fiber pointer to a future,
        /// and release the same here
        ///
        auto vv = m_future.get();
        m_state = skein_state::STATE_DONE;
        m_value_p.swap(vv);
        /// in boost fiber, the notify_all call will
        /// itself acquire the condition's mutex
        m_cond.notify_all();
        /// release m_lock
      }
      /// release m_cv_lock
    };
    return m_value_p.get();
  };

  virtual skein_state get_state() {
    // FIXME add an overload get_state(timeout) for acquiring m_lock
    std::scoped_lock<condition_lock_type> lck(m_lock);
    if (m_future.valid()) {
      return skein_state::STATE_ACQUIRED;
    }
    // state: pending or done
    return m_state;
  }

  virtual bool done() noexcept { return !static_cast<bool>(m_state); }

protected:
  future_type m_future;
  skein_state m_state;
  value_lock_type m_lock;
  condition_lock_type m_cv_lock;
  condition_type m_cond;
  ptr_type m_value_p;

  virtual value_type func() = 0;

  // FIXME this needs to support a deleter param for the ptr
  virtual ptr_type func_p() { return std::make_unique<T>(func()); }
};

} // namespace evtlet
