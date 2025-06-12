#pragma once

#ifdef USE_BEMAN_OPTIONAL
#include <beman/optional/optional.hpp>
#define OPTIONAL_T beman::optional::optional
#define NULLOPT beman::optional::nullopt
#else
#include <optional>
#define OPTIONAL_T std::optional
#define NULLOPT std::nullopt
#endif
