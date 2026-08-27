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

#include "clihehe/error.h"
#include "clihehe/extras/string_view.h"
#include <functional>
#include <string_view>
#include <type_traits>

namespace clihehe {
class CLIParserBase;

namespace nao {
template <class TParser> class OptBase {
protected:
  OptBase(std::string_view ArgLong, std::string_view Desc = "")
      : ArgLong(ArgLong), Desc(Desc) {};

  OptBase(char8_t C, std::string_view ArgLong, std::string_view Desc = "")
      : ArgShort(C), ArgLong(ArgLong), Desc(Desc) {};

  ~OptBase() = default;

public:
  TParser Par{};
  std::string_view ArgLong;
  std::string_view Desc;
  std::string_view VDesc;
  char8_t ArgShort;
  std::function<CLIError(OptBase &This)> Callback;
};
} // namespace nao

template <typename T, class Parser>
class Opt : public T, public nao::OptBase<Parser> {};

template <typename T, class Parser>
  requires std::is_class_v<T>
class Opt<T, Parser> : public T, public nao::OptBase<Parser> {
  using BaseAlias = nao::OptBase<Parser>;

  friend CLIParserBase;

public:
protected:
  Opt(std::string_view ArgLong, std::string_view Desc = "")
      : BaseAlias(ArgLong, Desc) {}

  template <typename... Ts>
  Opt(std::string_view ArgLong, std::string_view Desc, Ts &&...Args)
      : BaseAlias(ArgLong, Desc), BaseAlias::Par(std::forward<Ts>(Args)...) {}

private:
};
} // namespace clihehe

#endif
