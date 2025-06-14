
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <evtlet/evt/evt_cc.hpp>
#include <evtlet/evt/evt_fiber.hpp>

template <typename T> using evt_base = evtlet::evt_fiber<T>;

template <typename T> class test_evt : public evt_base<T> {
public:
  test_evt(bool immed = false)
      : evt_base<T>(immed ? evtlet::scheduling::SCHED_IMMED
                          : evtlet::scheduling::SCHED_DEFER) {}

  void test_state() {
    // test for consistency of fiber_pending() and get_state() monitors (one
    // fiber)
    REQUIRE(this->fiber_pending());
    REQUIRE(static_cast<size_t>(this->get_state()));
    REQUIRE(static_cast<size_t>(evtlet::evt_state::STATE_PENDING));
    auto disp_id = this->get_id();
    if (disp_id.has_value()) {
      auto s_detached = this->get_detached_state();
      // assert that the fiber is dispatching
      //
      // One possible gotcha: static_cast<bool>(s_detached)
      // does not indicate that any dispatching fiber has
      // detached. It should indicate only that there was
      // a dispatching fiber, when get_detached_state()
      // was called.
      //
      REQUIRE(s_detached);
      // assert that the dispatching fiber was detached
      // at the instant when get_detached_state() was
      // called
      REQUIRE(!static_cast<bool>(s_detached.value()));

      // test for a non-detached fiber
      //
      // a detached fiber has no ID and "is not a fiber"
      // its id will not be static_cast<bool>(id) true
      REQUIRE(boost::this_fiber::get_id() == disp_id.value());
    } else {
      auto s_detached = this->get_detached_state();
      REQUIRE(s_detached);
      REQUIRE(static_cast<bool>(s_detached.value()));
    }
  }
};

class pass_int : public test_evt<int> {
public:
  pass_int(int value, bool immed = false) : test_evt(immed), m_value(value) {};

protected:
  using base = test_evt<int>;
  const int m_value;
  virtual int func() {
    test_state();
    return m_value;
  };
};

class sk_void : public test_evt<void *> {

public:
  using test_evt<void *>::test_evt;

protected:
  using base = test_evt<void *>;
  virtual void *func() {
    test_state();
    return this;
  };
};

TEST_CASE("test dispatch, get") {
  SECTION("pass_int: dispatch, get") {
    int ivl = -5;
    auto ifiber = pass_int(ivl);
    REQUIRE(!static_cast<bool>(ifiber.get_state()));
    REQUIRE(!ifiber.has_value());
    ifiber.dispatch();
    REQUIRE(ifiber.get_state() == evtlet::evt_state::STATE_DONE);
    REQUIRE(ifiber.has_value());
    auto vv = ifiber.get();
    REQUIRE(vv == ivl);
  }

  SECTION("sk_void: dispatch, get") {
    auto vfiber = std::make_unique<sk_void>();
    REQUIRE(!static_cast<bool>(vfiber->get_state()));
    REQUIRE(!vfiber->has_value());
    vfiber->dispatch();
    REQUIRE(vfiber->get_state() == evtlet::evt_state::STATE_DONE);
    REQUIRE(vfiber->has_value());
    auto vv = vfiber->get();
    REQUIRE(vv == vfiber.get());
  }
}

TEST_CASE("test immediate dispatch") {
  SECTION("pass_int: immediate dispatch, has_value, get") {
    int ivl = -5;
    auto ifiber = pass_int(ivl, true);
    REQUIRE(static_cast<bool>(ifiber.get_state()));
    /// call yield() for the current fiber, before testing has_value()
    ///
    /// for the present design, this yield() call would not be necessary
    /// when calling immediately to get(), here or in the next test section
    ///
    /// considering that this yeild() call appears to be required here, it
    /// may be a side effect of locking for state field access in the evt 
    /// API at present (this will need further study)
    ///
    boost::this_fiber::yield();
    REQUIRE(ifiber.has_value());
    auto vv = ifiber.get();
    REQUIRE(vv == ivl);
  }

  SECTION("sk_void: immediate dispatch, has_value, get") {
    auto vfiber = std::make_unique<sk_void>(true);
    REQUIRE(static_cast<bool>(vfiber->get_state()));
    boost::this_fiber::yield();
    REQUIRE(vfiber->has_value());
    auto vv = vfiber->get();
    REQUIRE(vv == vfiber.get());
  }

  SECTION("pass_int: immediate dispatch, get") {
    int ivl = -5;
    auto ifiber = pass_int(ivl, true);
    REQUIRE(static_cast<bool>(ifiber.get_state()));
    auto vv = ifiber.get();
    REQUIRE(vv == ivl);
  }

  SECTION("sk_void: immediate dispatch, get") {
    auto vfiber = std::make_unique<sk_void>(true);
    REQUIRE(static_cast<bool>(vfiber->get_state()));
    auto vv = vfiber->get();
    REQUIRE(vv == vfiber.get());
  }
}
