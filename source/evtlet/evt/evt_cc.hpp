

#include <boost/context/continuation.hpp>
#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/mutex.hpp>

#include <evtlet/evt/evt.hpp>
#include <evtlet/evt/evt_state.hpp>
#include <evtlet/util/dbg.hpp>
#include <evtlet/util/optional_source.hpp>
#include <evtlet/util/scope_source.hpp>


namespace evtlet {

namespace ctx = boost::context;

using namespace std::placeholders;


template <typename T> class evt_cc : public evt<T> {
public:
  using value_t = T;
  using condition_lock_t = boost::fibers::mutex;
  using condition_t = boost::fibers::condition_variable;
  using co_t = ctx::continuation;

  template <typename S>
  using optional_t = OPTIONAL_T<S>;

private:
  optional_t<evt_state> m_state;
  optional_t<value_t> m_value;
  condition_lock_t m_cv_lock;
  condition_t m_cond;

public:
  evt_cc() : evt<T>() {};

  virtual void dispatch() {
    DBGMSG("dispatch_cc ...");
    auto &&ff = std::bind(&evt_cc<T>::dispatch_cc, this, _1);
    ctx::callcc(std::move(ff));
    DBGMSG("... dispatch_cc");
  }

  virtual void wait() {
    DBGMSG("wait_cc ...");
    auto ff = std::bind(&evt_cc<T>::wait_cc, this, _1);
    ctx::callcc(std::move(ff));
    DBGMSG("... wait_cc");
  }

  virtual T &get() {

    m_cv_lock.lock();
    bool locked = true;
    SCOPE_FAIL guard{[this, &locked]() {
      if (locked) {
        m_cv_lock.unlock();
        locked = false;
      }
    }}; // guard

    if (m_value.has_value()) {
      m_cv_lock.unlock();
      locked = false;
      return m_value.value();
    } else {
      m_cv_lock.unlock();
      locked = false;
      dispatch();
      ASSERT(m_value.has_value());
      return m_value.value();
    }
  }

  virtual bool done() noexcept { return m_value.has_value(); }

protected:
  virtual co_t dispatch_cc(co_t &&sink) {
    _acquire_cv_lock();
    bool locked = true;
    SCOPE_FAIL guard{[&locked, this]() {
      if (locked) {
        _release_cv_lock();
        locked = false;
      }
    }};
    if (_get_state() == evt_state::STATE_NONE) {
      _release_cv_lock();
      locked = false;
      auto ff = std::bind(&evt_cc<T>::func_cc, this, _1);
      ctx::callcc(ff);
    } else {
      _release_cv_lock();
      locked = false;
      wait();
    }
    return std::move(sink);
  }

  virtual co_t func_cc(co_t &&sink) {
    this->_set_state(evt_state::STATE_PENDING);
    DBGMSG("func_cc()");
    auto vv = this->func();

    this->_acquire_cv_lock();
    bool locked = true;
    SCOPE_FAIL guard{[&locked, this]() {
      if (locked) {
        this->_release_cv_lock();
        locked = false;
      }
    }};
    this->_set_value(std::move(vv));
    ASSERT(this->_get_state() != evt_state::STATE_DONE);
    this->_set_state(evt_state::STATE_DONE);
    this->_release_cv_lock();
    locked = false;
    // notify anything waiting on the cv lock
    this->_cv_notify();
    return std::move(sink);
  }

  virtual co_t wait_cc(co_t &&sink) {
    DBGMSG("in wait ... resume");
    co_t &&nxt = sink.resume();
    DBGMSG("... in wait ... resuming");
    std::unique_lock<condition_lock_t> cv_lck(m_cv_lock);
    while (!m_value.has_value()) {
      m_cond.wait(cv_lck);
    };
    DBGMSG("... end of wait");

    return std::move(nxt);
  }

  // protected accessors

  virtual evt_state _get_state() {
    return m_state.value_or(evt_state::STATE_NONE);
  }
  virtual void _set_state(evt_state st) { m_state.emplace(std::move(st)); }

  virtual void _set_value(T &&value) { m_value.emplace(value); }

  virtual void _acquire_cv_lock() { m_cv_lock.lock(); }

  virtual void _release_cv_lock() { m_cv_lock.unlock(); }

  virtual void _cv_notify() {
    m_cond.notify_all();
  }

};

} // namespace evtlet
