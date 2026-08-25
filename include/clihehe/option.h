//============================================================================//
// clihehe/option.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Options
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_OPTION_H
#define NAO_CLIHEHE_OPTION_H

#include <functional>
#include <string_view>
#include <type_traits>

namespace clihehe {
class ParserContext;

namespace nao {
class OptBase {
public:
  std::string_view ArgLong;
  std::string_view Desc;
  std::string_view VDesc;
  char8_t ArgShort;
  std::function<bool(ParserContext const &Ctx)> Callback;
};
} // namespace nao

template <typename T> class Opt {};

template <typename T>
  requires std::is_class_v<T>
class Opt<T> : public T {
private:
public:
};
} // namespace clihehe

#endif
