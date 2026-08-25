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
#include <concepts>

namespace clihehe {
namespace nao {
template <typename T> struct unkown_parser_for;
} // namespace nao

template <typename T> struct type_parser : nao::unkown_parser_for<T> {};

template <typename T>
concept ValidTypeParser = requires(T A) {
  { type_parser<T>::parse(A) } -> std::same_as<CLIError>;
};
} // namespace clihehe

#endif
