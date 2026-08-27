//============================================================================//
// clihehe/option_parser.h
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

#ifndef NAO_CLIHEHE_OPTIONPARSER_H
#define NAO_CLIHEHE_OPTIONPARSER_H

#include "clihehe/error.h"
#include "clihehe/parser_context.h"
#include <concepts>
#include <span>

namespace clihehe {
namespace nao {
template <typename T> struct unkown_parser_for;
} // namespace nao

template <typename T> struct type_parser : nao::unkown_parser_for<T> {};

template <typename T>
concept ValidSingleTypeParser = requires(T &Val, std::string_view C) {
  { type_parser<T>::parse(Val, C) } -> std::same_as<CLIError>;
};
} // namespace clihehe

#endif
