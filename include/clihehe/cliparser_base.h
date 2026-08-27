//============================================================================//
// clihehe/cliparser_base.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Base class for CLI parser
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_CLIPARSERBASE_H
#define NAO_CLIHEHE_CLIPARSERBASE_H

#include "clihehe/error.h"
#include "clihehe/option.h"
#include <string_view>

namespace clihehe {
class Subcommand;
class OptionCategory;

class CLIparserbase {
public:
  ~CLIparserbase() noexcept = default;

  CLIError parse_option(int argc, char const *const *argv);

  template <typename... T, typename... Ys>
  NAO_CLIHH_INLINE Opt<T...> get_option(Ys &&...Args) {
    return {std::forward<Ys>(Args)...};
  }

  std::string_view Name;
  std::string_view Desc;

protected:
  CLIparserbase(std::string_view Name, std::string_view Desc)
      : Name(Name), Desc(Desc) {}
};
} // namespace clihehe

#endif
