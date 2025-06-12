#pragma once

#ifdef USE_SCOPE_GUARD
#ifndef SCOPE_EXIT
#include <scope_exit.h>
#define SCOPE_EXIT sr::scope_exit
#endif
#ifndef SCOPE_FAIL
#include <scope_fail.h>
#define SCOPE_FAIL sr::scope_fail
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
