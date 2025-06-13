/**
 * @brief towards a bitmask interface for scoped enum types
 */

#pragma once

#include <type_traits>

/**
 * @brief tag class for a bitmask interface to enums
 * @tparam E enum type
 *
 * __Example:__ Define an enum `state_t` with underlying type `size_t` and
 * some enum values.
 *
 * ```cpp
 * DEFMASK_T(state_t, size_t, ST_NEVER = 0, ST_NOW = 1, ST_DONE = 2);
 * ```
 *
 * The enum `state_t` will then be defined such that
 * `enum_mask_t<state_t>::enable == true`. As such, operators for `==`, `|`,
 * `&`, and `^` will be defined for any `state_t` and for either a corresponding
 * `size_t` or `int` value.
 *
 * When `BOOST_ASSERT` is defined, the `int` operator will use the
 * `BOOST_ASSERT` macro, such as to ensure that the provided `int` value is not
 * less than zero.
 *
 * With this example, the unary operator `!` will also be defined for any
 * `state_t`, such that `!!value` may be used to convert a `state_t value` to
 * a boolean representation of the equivalent `size_t` value.
 *
 * __References__
 *
 * - Anthony Williams. 2016. Using Enum Classes as Bitfields. _Overload_,
 *   24(132), 22–23. https://accu.org/journals/overload/24/132/williams_2228/
 *
 * - Johannes Schaub. 2012. _Stack Overflow_
 * https://stackoverflow.com/a/9877317/1061095
 */
template <typename E> struct enum_mask_t {
  static constexpr bool enable = false;
};

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator|(E lhs, E rhs) {
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) |
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator|(E lhs, int rhs) {
#ifdef BOOST_ASSERT
  BOOST_ASSERT(rhs >= 0);
#endif
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) |
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator^(E lhs, E rhs) {
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) ^
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator^(E lhs, int rhs) {
#ifdef BOOST_ASSERT
  BOOST_ASSERT(rhs >= 0);
#endif
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) ^
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator&(E lhs, E rhs) {
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) &
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, E>::type
operator&(E lhs, int rhs) {
#ifdef BOOST_ASSERT
  BOOST_ASSERT(rhs >= 0);
#endif
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E>(static_cast<underlying>(lhs) &
                        static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, bool>::type
operator==(E lhs, std::underlying_type<E> rhs) {
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<bool>(static_cast<underlying>(lhs) == rhs);
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, bool>::type
operator==(E lhs, int rhs) {
#ifdef BOOST_ASSERT
  BOOST_ASSERT(rhs >= 0);
#endif
  return static_cast<int>(lhs) == rhs;
}

template <typename E>
constexpr typename std::enable_if<enum_mask_t<E>::enable, bool>::type
operator!(E lhs) {
  typedef typename std::underlying_type<E>::type underlying;
  return lhs == static_cast<underlying>(0);
}

#ifndef DEFMASK_T
#define DEFMASK_T(NAME, TYPE, ...)                                             \
  enum class NAME : TYPE { __VA_ARGS__ };                                      \
  template <> struct enum_mask_t<NAME> {                                       \
    static constexpr bool enable = true;                                       \
  };
#endif
