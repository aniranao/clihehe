//============================================================================//
// clihehe/extras/RangesExtras.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Ranges, but ++
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_RANGESEXTRAS_H
#define NAO_CLIHEHE_RANGESEXTRAS_H

#include "clihehe/macro.h"
#include <algorithm>
#include <ranges>
#include <version>

namespace clihehe::nao {
#ifdef __cpp_lib_ranges_contains
template <typename... Ts>
NAO_CLIHNAO_CLIHH_ALWAYS_INLINE_ATTR auto ranges_contains(Ts &&...Args) {
  return std::ranges::contains(std::forward<Ts>(Args)...);
}
#else
namespace detail {
template <class S, typename T>
concept HasContains = requires(S &&O, T &&A) {
  { std::forward<S>(O).contains(std::forward<T>(A)) } -> std::same_as<bool>;
};

template <class S, typename T>
concept HasFind = requires(S &&O, T &&A) {
  {
    std::forward<S>(O).find(std::forward<T>(A)) != std::forward<S>(O).end()
  } -> std::same_as<bool>;
};
} // namespace detail

template <typename R, typename T> bool ranges_contains(R &&Ranges, const T &E) {
  if constexpr (detail::HasContains<R, T>) {
    return std::forward<R>(Ranges).contains(E);
  } else if constexpr (detail::HasFind<R, T>) {
    return std::forward<R>(Ranges).find(E) != std::forward<R>(Ranges).end();
  } else {
    return std::ranges::find(std::forward<R>(Ranges), E) !=
           std::ranges::end(Ranges);
  }
}
#endif
} // namespace clihehe::nao

#endif
