#pragma once

#ifdef BEMAN_SCOPE
#include <beman/scope/scope.hpp>
#define SCOPE_EXIT beman::scope::scope_exit
#define SCOPE_FAIL beman::scope::scope_fail
#else
#include <experimental/scope>
#define SCOPE_EXIT std::experimental::scope_exit
#define SCOPE_EXIT std::experimental::scope_fail
#endif
