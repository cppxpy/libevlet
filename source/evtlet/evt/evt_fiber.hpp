/**
 * @file evt_cond.hpp
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

#include <evtlet/evt/evt.hpp>
#include <evtlet/evt/evt_state.hpp>
#include <evtlet/util/dbg.hpp>
#include <evtlet/util/optional_source.hpp>
#include <evtlet/util/scope_source.hpp>

namespace evtlet {

enum class scheduling : size_t {
  SCHED_NEVER = 0,
  SCHED_DEFER = 1,
  SCHED_IMMED = 2
};

/**
 * @brief prototype for an Event-oriented API onto boost fibers
 *
 * @tparam T return value type for the event function
 */
template <typename T, typename condition_lock_t = boost::fibers::mutex,
          typename condition_t = boost::fibers::condition_variable>
class evt_fiber : public evt<T> {
public:
  using fiber_t = boost::fibers::fiber;
  using fiber_id_t = fiber_t::id;
  using value_t = T;

private:
  static void _dealloc_fiber(fiber_t *fiber) {
    // deleter function for the m_fiber pointer
    if (fiber->joinable()) {
      fiber->detach();
    }
    delete fiber;
  }

  const scheduling m_sched;
  OPTIONAL_T<evt_state> m_state;
  OPTIONAL_T<value_t> m_value;
  std::unique_ptr<fiber_t, std::function<void(fiber_t *)>> m_fiber;

  condition_lock_t m_cv_lock;
  condition_t m_cond;

public:
  explicit evt_fiber(scheduling sched = scheduling::SCHED_DEFER)
      : evt<T>(), m_sched(sched), m_state(), m_value(NULLOPT),
        m_fiber(nullptr, _dealloc_fiber) {
    if (static_cast<size_t>(sched) &
        static_cast<size_t>(scheduling::SCHED_IMMED)) {
      dispatch_detached();
    }
  };

  virtual ~evt_fiber() {
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
    bool locked = true;
    SCOPE_FAIL guard{[this, &locked]() {
      if (locked) {
        m_cv_lock.unlock();
        locked = false;
      }
    }}; // guard

    if (static_cast<bool>(m_value)) {
      m_cv_lock.unlock();
      locked = false;
      return m_value.value();
    } else {
      DBGMSG("wait_fiber: new");
      boost::fibers::fiber wait_fiber([this]() {
        DBGMSG("wait_fiber: initialized");
        if (!static_cast<bool>(m_state)) {
          auto df = ensure_fiber();
          ASSERT(df);
          if (df->joinable()) {
            DBGMSG("wait_fiber: yield to ensure_fiber fiber");
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
      locked = false;
      wait_fiber.join();
      DBGMSG("wait_fiber: returned");
      return m_value.value();
    }
  };

  virtual void dispatch() {
    auto *ff = ensure_fiber();
    if (ff && ff->joinable()) {
      ff->join();
    }
  }

  virtual void dispatch_detached() {
    auto *ff = ensure_fiber();
    if (ff && ff->joinable()) {
      ff->detach();
    }
  }

  virtual fiber_t *ensure_fiber() {
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
      m_state.emplace(evt_state::STATE_PENDING);
      DBGMSG("ensure_fiber fiber: new");
      fiber_t *disp =
          new fiber_t(std::bind(&evt_fiber<T>::dispatch_func, this));
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

  virtual bool has_value() noexcept { return m_value.has_value(); }

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

  virtual evt_state get_state() {
    m_cv_lock.lock();
    SCOPE_EXIT guard{[this]() { m_cv_lock.unlock(); }};
    return static_cast<bool>(m_fiber)
               ? (m_value.has_value()
                      // has a fiber and value
                      ? evt_state::STATE_DONE
                      // has a fiber, no value
                      : (static_cast<bool>(m_fiber->get_id())
                             ? evt_state::STATE_PENDING
                             : evt_state::STATE_PENDING_DETACHED))
               // has no fiber at this time
               : evt_state::STATE_NONE;
  }

protected:
  virtual void dispatch_func() {
    DBGMSG("dispatch_func: running");
    ASSERT(!m_value.has_value());
    auto vv = this->func();
    _cv_prelock_acquire();
    m_value.emplace(std::move(vv));
    ASSERT(m_state != evt_state::STATE_DONE);
    m_state.emplace(evt_state::STATE_DONE);
    ASSERT(m_fiber);
    if (m_fiber->joinable()) {
      // detach the fiber before potential deletion
      m_fiber->detach();
    };
    m_cond.notify_all();
    DBGMSG("dispatch_func: returning");
  };

  // protected accessors for subclasses
  virtual evt_state _get_state() {
    return m_state.value_or(evt_state::STATE_NONE);
  }
  virtual void _set_state(evt_state st) { m_state.emplace(st); }

  virtual void _set_value(T &&value) { m_value.emplace(value); }

  virtual void _acquire_cv_lock() { m_cv_lock.lock(); }

  virtual void _release_cv_lock() { m_cv_lock.unlock(); }

  virtual void _cv_prelock_acquire() {
    // some condition variable implementations will need the
    // condition's lock to be acquired before cond.notify()
    //
    // boost fiber conditions may acquire the lock eagerly,
    // such that this method may be a no-op here - though
    // it may be usable in this case, if (perhaps only if)
    // using a _recursive_ fiber mutex for the CV
  }
  virtual void cv_notify() {
    m_cond.notify_all();
  }
};


} // namespace evtlet
