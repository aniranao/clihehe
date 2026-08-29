//============================================================================//
// clihehe/extras/ranges.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Extra things for std::ranges
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_EXTRAS_RANGES_H
#define NAO_CLIHEHE_EXTRAS_RANGES_H

#include <ranges>
#include <version>

namespace clihehe::nao {
#ifdef __cpp_lib_ranges_contains
template <typename... Ts> [[nodiscard]] bool ranges_contains(Ts &&...Args) {
  return std::ranges::contains(std::forward<Ts>(Args)...);
}
#else
namespace detail {
template <typename R, typename T>
concept HasContains = requires(R &&Ran, const T &A) {
  { Ran.contains(A) } -> std::same_as<bool>;
};

template <typename R, typename T>
concept HasFind = requires(R &&Ran, const T &A) {
  { Ran.find(A) } -> std::same_as<decltype(Ran.end())>;
};
} // namespace detail

template <typename Ranges, typename R>
[[nodiscard]] bool ranges_contains(Ranges &&ranges, R const &V) {
  if constexpr (detail::HasContains<Ranges, R>) {
    return ranges.contains(V);
  } else if constexpr (detail::HasFind<Ranges, R>) {
    return ranges.find(V) != ranges.end();
  } else {
    return std::ranges::find(ranges) != std::ranges::end(ranges);
  }
}
#endif
} // namespace clihehe::nao

#endif
