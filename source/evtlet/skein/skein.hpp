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

#include <functional>
#include <memory>
#include <mutex>

#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/future/async.hpp>
#include <boost/fiber/future/future.hpp>
#include <boost/fiber/mutex.hpp>

#include <evtlet/util/optional_source.hpp>
#include <evtlet/util/scope_source.hpp>
#include <evtlet/util/dbg.hpp>

namespace evtlet {

enum class skein_state : size_t {
  STATE_NONE = 0,
  STATE_PENDING = 1,
  STATE_DETACHED = 2,
  STATE_DONE = 4,
  STATE_PENDING_DETACHED = STATE_PENDING | STATE_DETACHED
};

enum class scheduling : size_t { SCHED_DEFER = 0, SCHED_IMMED = 1 };


/**
 * @brief prototype for an Event-oriented API onto boost fibers
 *
 * @tparam T return value type for the event function
 */
template <typename T> class skein {
public:
  using value_t = T;
  using fiber_t = boost::fibers::fiber;
  // fiber_id_t : here N/A for both detached-but-running and invalid fibers
  using fiber_id_t = boost::fibers::fiber::id;
  // value_lock_t: exclusive only for other fibers within this thread
  using value_lock_t = boost::fibers::mutex;
  // condition_lock_t: similarly fiber-exclusive and not thread-exclusive
  using condition_lock_t = boost::fibers::mutex;
  using condition_t = boost::fibers::condition_variable;

private:
  static void _dealloc_fiber(fiber_t *fiber) {
    // deleter function for the m_fiber pointer
    if (fiber->joinable()) {
      fiber->detach();
    }
    delete fiber;
  }

  const scheduling m_sched;
  OPTIONAL_T<skein_state> m_state;
  OPTIONAL_T<value_t> m_value;
  std::unique_ptr<fiber_t, std::function<void(fiber_t *)>> m_fiber;

  condition_lock_t m_cv_lock;
  condition_t m_cond;

public:
  skein(scheduling sched = scheduling::SCHED_DEFER)
      : m_sched(sched), m_state(), m_value(NULLOPT),
        m_fiber(nullptr, _dealloc_fiber) {
    if (static_cast<size_t>(sched) &
        static_cast<size_t>(scheduling::SCHED_IMMED)) {
      auto opt_fb = dispatch();
      ASSERT(opt_fb);
      ASSERT(opt_fb->joinable());
      opt_fb->detach();
      ASSERT(!opt_fb->joinable());
    }
  };

  /// no copy
  skein(skein const &) = delete;

  /// no assign
  skein &operator=(skein const &) = delete;

  virtual ~skein() {
    if (m_fiber && m_fiber->joinable()) {
      m_fiber->detach();
    }
    m_fiber.release();
  };

  virtual void wait() {
    std::unique_lock<condition_lock_t> cv_lck(m_cv_lock);
    DBGMSG("in wait ...");
    while (!static_cast<bool>(m_value)) {
      m_cond.wait(cv_lck);
      DBGMSG("... returning from wait");
    }
    DBGMSG("... end of wait");
  };

  virtual T &get() {

    m_cv_lock.lock();

    if (static_cast<bool>(m_value)) {
      m_cv_lock.unlock();
      return m_value.value();
    } else {
      DBGMSG("wait_fiber: new");
      boost::fibers::fiber wait_fiber([this]() {
        DBGMSG("wait_fiber: initialized");
        if (!static_cast<bool>(m_state)) {
          auto df = dispatch();
          ASSERT(df);
          if (df->joinable()) {
            DBGMSG("wait_fiber: yield to dispatch fiber");
            // if m_fiber was reset after call, this could be reached
            // i.e it might still appear joinable() but the join() may
            // segfault then
            df->join();
            DBGMSG("wait_fiber: dispatched");
          } else {
            DBGMSG("wait_fiber: already disapatching!");
          }
        };
        DBGMSG("wait_fiber: wait");
        wait();
        DBGMSG("wait_fiber: returning");
      });
      m_cv_lock.unlock();
      wait_fiber.join();
      DBGMSG("wait_fiber: returned");
      return m_value.value();
    }
  };

  virtual fiber_t *dispatch() {
    // if m_fiber is null, then create a new fiber and store in m_fiber
    m_cv_lock.lock();
    bool locked = true;
    SCOPE_FAIL guard{[this, &locked]() {
      if (locked) {
        m_cv_lock.unlock();
        locked = false;
      }
    }}; // guard
    if (!m_fiber) {
      m_state.emplace(skein_state::STATE_PENDING);
      DBGMSG("dispatch fiber: new");
      fiber_t *disp = new fiber_t(std::bind(&skein<T>::dispatch_func, this));
      m_fiber.reset(std::move(disp));
    };
    m_cv_lock.unlock();
    locked = false;
    return m_fiber.get();
  };

  virtual bool done() noexcept { return static_cast<bool>(m_value); };

  virtual bool fiber_pending() noexcept {
    return static_cast<bool>(m_fiber) && !m_value.has_value();
  }

  virtual OPTIONAL_T<bool> get_detached_state() {
    m_cv_lock.lock();
    SCOPE_EXIT guard{[this]() { m_cv_lock.unlock(); }};
    OPTIONAL_T<bool> opt_id{NULLOPT};
    if (m_fiber) {
      const auto id = m_fiber->get_id();
      // this may indicate either a detached fiber
      // or a fiber in unknown state (e.g post-move)
      return !static_cast<bool>(id);
    };
    return opt_id;
  }

  virtual OPTIONAL_T<fiber_id_t> get_id() {
    m_cv_lock.lock();
    SCOPE_EXIT guard{[this]() { m_cv_lock.unlock(); }};
    OPTIONAL_T<fiber_id_t> opt_id{NULLOPT};
    if (m_fiber) {
      // update opt_id when m_fiber has a value
      const auto id = m_fiber->get_id();
      if (static_cast<bool>(id)) {
        // fiber is in a known state
        // i.e not detached, thus has an id
        opt_id.emplace(std::move(id));
      }
    }
    return opt_id;
  }

  virtual skein_state get_state() {
    m_cv_lock.lock();
    SCOPE_EXIT guard{[this]() { m_cv_lock.unlock(); }};
    return static_cast<bool>(m_fiber)
               ? (m_value.has_value()
                      // has a fiber and value
                      ? skein_state::STATE_DONE
                      // has a fiber, no value
                      : (static_cast<bool>(m_fiber->get_id())
                             ? skein_state::STATE_PENDING
                             : skein_state::STATE_PENDING_DETACHED))
               // has no fiber at this time
               : skein_state::STATE_NONE;
  }

protected:
  virtual value_t func() = 0;

  virtual void dispatch_func() {
    DBGMSG("dispatch_func: running");
    ASSERT(!m_value.has_value());
    auto vv = func();
    cv_lock_acquire();
    m_value.emplace(std::move(vv));
    ASSERT(m_state != skein_state::STATE_DONE);
    m_state.emplace(skein_state::STATE_DONE);
    ASSERT(m_fiber);
    if (m_fiber->joinable()) {
      // detach the fiber before potential deletion
      m_fiber->detach();
    };
    m_cond.notify_all();
    DBGMSG("dispatch_func: returning");
  };

  virtual void cv_lock_acquire() {
    // some condition variable implementations will need the
    // condition's lock to be acquired before cond.notify()
    //
    // boost fiber conditions may acquire the lock eagerly,
    // such that this method may be a no-op here - though
    // it may be usable in this case, if (perhaps only if)
    // using a _recursive_ fiber mutex for the CV
  }
};

} // namespace evtlet
