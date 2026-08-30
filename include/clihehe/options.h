//============================================================================//
// clihehe/options.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// All options avaiable for arg<T>, positional<T>
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_OPTIONS_H
#define NAO_CLIHEHE_OPTIONS_H

#include <concepts>
#include <string_view>

namespace clihehe::st {
struct hidden {
  template <typename O> void apply(O *Opt) { Opt->hidden(); }
};

struct desc {
  std::string_view Str;

  explicit desc(std::string_view Str) : Str(Str) {}

  template <typename O> void apply(O *Opt) { Opt->setDesc(Str); }
};

struct long_opt {
  std::string_view Str;

  explicit long_opt(std::string_view Str) : Str(Str) {}

  template <typename O> void apply(O *Opt) { Opt->setLongOpt(Str); }
};

struct value_desc {
  std::string_view Str;

  explicit value_desc(std::string_view Str) : Str(Str) {}

  template <typename O> void apply(O *Opt) { Opt->setValueDesc(Str); }
};

template <typename T> struct default_val {
  T Val;

  template <typename R>
    requires std::convertible_to<R, T>
  explicit default_val(R &&Val) : Val(Val) {}

  template <typename O>
    requires std::convertible_to<T, typename O::OptValueType>
  void apply(O *Opt) {
    Opt->setDefault(Val);
  }
};
} // namespace clihehe::st

#endif
