/**
 * @file dbg.hpp
 * @brief debugging utilities
 *
 * @copyright Copyright (c) 2025 Sean Champ
 *
 */

#pragma once

#ifndef DBGMSG
#ifdef NDEBUG
#define DBGMSG(first, ...)
#else
template <typename Arg, typename... Args>
void __dbg_out(Arg &&arg, Args &&...args) {
  // using c++17 fold expressions for iteration on args
  std::cerr << std::forward<Arg>(arg);
  ((std::cerr << std::forward<Args>(args)), ...);
  std::cerr << std::endl;
}
#define DBGMSG(first, ...) __dbg_out(first __VA_OPT__(, ) __VA_ARGS__)
#endif
#endif

#ifdef ASSERT_LIBASSERT
#include "libassert/assert.hpp"
#else
#include <boost/assert.hpp>
#ifndef ASSERT
#define ASSERT(expr, ...) BOOST_ASSERT(expr)
#endif
#ifndef DEBUG_ASSERT
#define DEBUG_ASSERT(expr, ...) BOOST_ASSERT(expr)
#endif
#ifndef ASSUME
#define ASSUME(expr, ...) BOOST_ASSERT(expr)
#endif
#endif
