//============================================================================//
// clihehe/extras/string_view.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Extra things for std::string_view
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_EXTRAS_STRINGVIEW_H
#define NAO_CLIHEHE_EXTRAS_STRINGVIEW_H

#include <cstddef>
#include <string_view>

namespace clihehe::nao {
struct string_literal : public std::string_view {
private:
  consteval string_literal(const char *A, size_t N) : std::string_view(A, N) {}

public:
  template <size_t N>
  consteval string_literal(const char (&A)[N]) : string_literal(A, N - 1) {}

  template <size_t N>
  [[nodiscard]] consteval static string_literal with_null(const char (&A)[N]) {
    return {A, N};
  }
};
} // namespace clihehe::nao

#endif
