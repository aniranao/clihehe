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

#include <span>
#include <string_view>

namespace clihehe {
struct ParserContext {
  std::string_view programName;
  std::span<std::string_view const> Args;
  size_t I;
};
} // namespace clihehe

#endif
