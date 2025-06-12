/**
 * @file test_evt_cond.cpp
 * @brief trivial test case for a prototype onto boost fiber condition variables
 *
 */

#include <iostream>

#include <evtlet/evt/evt_fiber.hpp>


template <typename T> class beta_evt : public evtlet::evt<T> {
public:
  using evtlet::evt<T>::evt;
  void test_state() {
    // test for consistency of fiber_pending() and get_state() monitors (one fiber)
    ASSERT(this->fiber_pending());
    ASSERT(static_cast<size_t>(this->get_state()) && static_cast<size_t>(evtlet::evt_state::STATE_PENDING));
    auto disp_id = this->get_id();
    if(disp_id.has_value()) {
      auto s_detached = this->get_detached_state();
      ASSERT(s_detached);
      ASSERT(!static_cast<bool>(s_detached.value()));

      // test for a non-detached fiber
      //
      // a detached fiber has no ID and "is not a fiber"
      // its id will not be static_cast<bool>(id) true
      ASSERT(boost::this_fiber::get_id() == disp_id.value());
    } else {
      auto s_detached = this->get_detached_state();
      ASSERT(s_detached);
      ASSERT(static_cast<bool>(s_detached.value()));
    }
  }

protected:
  // m_fiber is inaccessible also here
  // void thunk() { ASSERT(boost::this_fiber::get_id() ==
  // this->m_fiber->get_id()); }
};

class echo_int : public beta_evt<int> {
public:
  echo_int(int value, bool immed = false)
      : beta_evt(immed ? evtlet::scheduling::SCHED_IMMED
                         : evtlet::scheduling::SCHED_DEFER),
        m_value(value) {};

protected:
  using base = beta_evt<int>;
  const int m_value;
  virtual int func() {
    test_state();
    std::cerr << "<<echo_int::func>>" << std::endl;
    return m_value;
  };
};

class sk_void : public beta_evt<void *> {

protected:
  using base = beta_evt<void *>;
  virtual void *func() {
    test_state();
    std::cerr << "<<sk_void::func>>" << std::endl;
    return this;
  };
};

auto main() -> int {

  // test for one way to check after move ...
  beta_evt<int>::fiber_t fb{[]() {
    return 12321;
  }};
  auto other = std::move(fb);
  assert(!static_cast<bool>(fb.get_id()));
  assert(!fb.joinable());
  // it's ambiguous though, as that test is also true in the following:
  other.detach();
  assert(!static_cast<bool>(other.get_id()));
  // this is also true then:
  assert(!other.joinable());

  auto sk = new echo_int(5);
  ASSERT(sk->get_state() == evtlet::evt_state::STATE_NONE);
  // test with dispatch under get()
  auto vv = sk->get();
  ASSERT(!sk->fiber_pending());
  ASSERT(sk->get_state() == evtlet::evt_state::STATE_DONE);

  if (vv == 5) {
    std::cerr << "OK: ";
    std::cerr << vv;
    std::cerr << std::endl;
    // test for second dispatch()
    ASSERT(vv == sk->get());
    ASSERT(!sk->fiber_pending());
    ASSERT(sk->get_state() == evtlet::evt_state::STATE_DONE);
    delete (sk);

    DBGMSG("test immediate evt");
    auto isk = new echo_int(-5, true);
    ASSERT(isk->get_state() != evtlet::evt_state::STATE_NONE);
    ASSERT(isk->get() == -5);

    DBGMSG("test void evt");

    // test with dispatch() before get()
    auto vsk = new sk_void();
    ASSERT(vsk->get_state() == evtlet::evt_state::STATE_NONE);
    auto vdf = vsk->dispatch();
    ASSERT(vdf);
    vdf->join();
    auto vvsk = vsk->get();
    ASSERT(vsk->get_state() == evtlet::evt_state::STATE_DONE);
    ASSERT(vvsk);
    ASSERT(vvsk == vsk->get());
    delete (vsk);
  } else {
    std::cerr << "Unexpected Value: ";
    std::cerr << vv;
    std::cerr << std::endl;
    return 1;
  }

  return 0;
};
