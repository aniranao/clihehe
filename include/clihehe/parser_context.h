//============================================================================//
// clihehe/parser_context.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Type parser
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_PARSERCONTEXT_H
#define NAO_CLIHEHE_PARSERCONTEXT_H

#include "clihehe/error.h"
#include "clihehe/fmt.h"
#include <string_view>
#include <vector>

namespace clihehe {
struct ParserContext {
  std::string_view const programName;
  std::vector<std::string_view>::const_iterator Iterator;

  void operator++() { Iterator++; }
  void operator--() { Iterator--; }
  std::string_view const *operator->() const { return &*Iterator; }
  std::string_view operator*() const { return *Iterator; }

  template <typename... Ts>
  CLIError error(CLIErrorEnum E, NAO_CLIHH_FMT::format_string<Ts...> Fmt,
                 Ts &&...Args) const {
    return {programName, E, Fmt, std::forward<Ts>(Args)...};
  }
  static CLIError success() { return {}; }

  CLIError value_error(std::string_view Context = "") const {
    if (Context.empty()) {
      return {programName, CLIErrorEnum::InvalidValue, "Value error at '{}'",
              *Iterator};
    }

    return {programName, CLIErrorEnum::InvalidValue, "Value error at '{}': {}",
            *Iterator, Context};
  }
};
} // namespace clihehe

#endif
