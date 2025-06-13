
#include <catch2/catch_test_macros.hpp>

#ifdef BOOST_ASSERT
#undef BOOST_ASSERT
#endif
#define BOOST_ASSERT REQUIRE

#include <evtlet/util/mask.hpp>

DEFMASK_T(state_t, size_t, ST_NEVER = 0, ST_NOW = 1, ST_DONE = 2);

TEST_CASE("test DEFMASK_T(state_t, size_t, ...)") {

  auto t0 = state_t::ST_NEVER;
  REQUIRE(!static_cast<bool>(t0));

  auto t1 = state_t::ST_NOW;
  REQUIRE(static_cast<bool>(t1));
  REQUIRE(t1 == 1);

  auto t2 = state_t::ST_DONE;
  REQUIRE(static_cast<bool>(t2));
  REQUIRE(t2 == 2);

  auto t3 = state_t::ST_NOW | state_t::ST_DONE;
  // REQUIRE(static_cast<bool>(t3)); // not available
  REQUIRE(!!t3);
  REQUIRE(t3 == (2 | 1));

  auto t4 = state_t::ST_NOW | 2;
  REQUIRE(!!t4);
  REQUIRE(t4 == (2 | 1));
  REQUIRE(t4 == t3);

  auto t5 = t4 & 2;
  REQUIRE(!!t5);
  REQUIRE(t5 != t4);
  REQUIRE(t5 == 2);

  const auto t6 = t4 ^ 2;
  REQUIRE(!!t6);
  REQUIRE(t6 != t4);
  REQUIRE(t6 != t5);
  REQUIRE(t6 == 1);
}
