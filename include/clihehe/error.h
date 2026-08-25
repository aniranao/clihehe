//============================================================================//
// clihehe/error.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Simple CLI error
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_ERROR_H
#define NAO_CLIHEHE_ERROR_H

#include "clihehe/extras/errhandling.h"
#include "clihehe/extras/string_view.h"
#include "clihehe/fmt.h"
#include <string>
#include <system_error>
#include <type_traits>

#define NAO_CLIHH_DEBUG

namespace clihehe {
// NOLINTBEGIN(performance-enum-size)
enum class CLIErrorEnum { Nice = 0, InvalidOpt, InvalidValue, Unkown };
// NOLINTEND(performance-enum-size)

[[nodiscard]] nao::string_literal enum_to_mess(CLIErrorEnum E) {
  using enum CLIErrorEnum;

  switch (E) {
  case Nice:
    return "Nice!";
  case InvalidOpt:
    return "Invalid option";
  case InvalidValue:
    return "Invalid value";
  case Unkown:
    return "Unkown";
  }

  nao::fatal_error_report("Unkown error!");
}

class CLIErrorCategory : public std::error_category {
  // NOLINTBEGIN(modernize-use-nodiscard)
  const char *name() const noexcept override { return "cli_error_category"; }
  std::string message(int E) const override {
    return std::string(enum_to_mess(static_cast<CLIErrorEnum>(E)));
  }
  // NOLINTEND(modernize-use-nodiscard)
};

inline CLIErrorCategory const &getCLIErrorCategory() {
  static CLIErrorCategory const A;
  return A;
}

NAO_CLIHH_ALWAYS_INLINE_ATTR std::error_code make_error_code(CLIErrorEnum E) {
  return {static_cast<int>(E), getCLIErrorCategory()};
}

class [[nodiscard]] CLIError {
public:
  CLIError() = default;
  CLIError(std::string_view ProgramName, CLIErrorEnum E, std::string A = "")
      : ProgramName(ProgramName), E(E), Extra(std::move(A)) {}

  template <typename... Args>
  CLIError(std::string_view ProgramName, CLIErrorEnum E,
           NAO_CLIHH_FMT::format_string<Args...> Str, Args &&...args)
      : ProgramName(ProgramName), E(E) {
    Extra = NAO_CLIHH_FMT::format(Str, std::forward<Args>(args)...);
  }

  CLIError(const CLIError &) = delete;
  CLIError &operator=(CLIError const &) = delete;

  friend void
  swap(CLIError &A,
       CLIError &B) noexcept(std::is_nothrow_swappable_v<std::string>) {
    using std::swap;

    swap(A.ProgramName, B.ProgramName);
    swap(A.Extra, B.Extra);
    swap(A.E, B.E);
  }

  CLIError(CLIError &&O) noexcept { swap(*this, O); }
  CLIError &operator=(CLIError &&O) noexcept {
    swap(*this, O);
    return *this;
  }

  [[nodiscard]] std::string message() const {
    return NAO_CLIHH_FMT::format("{}: {}: {}", ProgramName, enum_to_mess(E),
                                 Extra);
  }

  operator bool() const { return E == CLIErrorEnum::Nice; }

private:
  std::string_view ProgramName;
  std::string Extra;
  CLIErrorEnum E = CLIErrorEnum::Nice;
};
} // namespace clihehe

namespace std {
template <>
struct is_error_code_enum<clihehe::CLIErrorEnum> : std::true_type {};
} // namespace std

#endif
