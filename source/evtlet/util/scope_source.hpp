#pragma once

#ifdef BEMAN_SCOPE
#include <beman/scope/scope.hpp>
#ifndef SCOPE_EXIT
#define SCOPE_EXIT beman::scope::scope_exit
#endif
#ifndef SCOPE_FAIL
#define SCOPE_FAIL beman::scope::scope_fail
#endif
#else
#include <experimental/scope>
#ifndef SCOPE_EXIT
#define SCOPE_EXIT std::experimental::scope_exit
#endif
#ifndef SCOPE_FAIL
#define SCOPE_FAIL std::experimental::scope_fail
#endif
#endif
