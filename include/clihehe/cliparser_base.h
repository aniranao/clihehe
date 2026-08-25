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
#include <string_view>

namespace clihehe {
class OptionBase;
class Subcommand;
class OptionCategory;

class CLIparserbase {
public:
  virtual ~CLIparserbase() noexcept = default;

  virtual CLIError parse_option(int argc, char const *const *argv) = 0;

  std::string_view Name;
  std::string_view Desc;

protected:
  CLIparserbase(std::string_view Name, std::string_view Desc)
      : Name(Name), Desc(Desc) {}
};
} // namespace clihehe

#endif
