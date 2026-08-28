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

#ifndef NAO_CLIHEHE_TYPES_PARSER_H
#define NAO_CLIHEHE_TYPES_PARSER_H

#include "clihehe/error.h"
#include "clihehe/extras/string_view.h"
#include <charconv>
#include <string>
#include <vector>

namespace clihehe {
template <nao::nttp_string Seq = ",", bool Space = true> struct parser {
  using ParserType = std::vector<int>;

  CLIError parse(ParserType &Vec, std::string_view Str) {
    if constexpr (std::string_view(Seq).empty() && Space) {
      nao::StringViewPP S = Str;
      for (auto [V, Etc] : std::tuple(S.split(" "))) {
        int A;
        auto [Ptr, EC] = std::from_chars(V.data(), V.data() + V.size(), A);
        if (Ptr != V.data() || EC != std::errc{}) {
          return CLIError{"Idk", CLIErrorEnum::InvalidValue,
                          "Unexpected value at '{}'", V};
        }

        Vec.push_back(A);
        S = Etc;
      }
    }
  }
};
} // namespace clihehe

#endif
