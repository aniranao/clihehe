//============================================================================//
// clihehe/base.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Base!
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_BASE_H
#define NAO_CLIHEHE_BASE_H

#include "macros.h"
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace clihehe {
enum class OccurrencesFlag : uint8_t {
  Optional,
  ZeroOrMore,
  Required,
  OneOrMore,
  ConsumeAfter
};

enum class ValueExpected : uint8_t { Optional, Required, Disallowed };
enum class TypeOption : uint8_t { Normal, Positional, Prefix };
enum class ValueMiscFlag : uint8_t {
  NONE = 0x0,

  CommaSeparated = 1 << 0, // for vector
  ConsumeFlag = 1 << 1,
  ConsumeUnkownFlag = 1 << 2,

  Default = (CommaSeparated | ConsumeFlag | ConsumeUnkownFlag)
};

constexpr ValueMiscFlag operator|(ValueMiscFlag LHS, ValueMiscFlag RHS) {
  using T = std::underlying_type_t<ValueMiscFlag>;
  return static_cast<ValueMiscFlag>(static_cast<T>(LHS) | static_cast<T>(RHS));
}

constexpr ValueMiscFlag &operator|=(ValueMiscFlag &LHS, ValueMiscFlag RHS) {
  using T = std::underlying_type_t<ValueMiscFlag>;
  return LHS = LHS | RHS;
}

constexpr ValueMiscFlag operator&(ValueMiscFlag LHS, ValueMiscFlag RHS) {
  using T = std::underlying_type_t<ValueMiscFlag>;
  return static_cast<ValueMiscFlag>(static_cast<T>(LHS) & static_cast<T>(RHS));
}

constexpr ValueMiscFlag &operator&=(ValueMiscFlag &LHS, ValueMiscFlag RHS) {
  using T = std::underlying_type_t<ValueMiscFlag>;
  return LHS = LHS & RHS;
}

constexpr ValueMiscFlag operator~(ValueMiscFlag RHS) {
  using T = std::underlying_type_t<ValueMiscFlag>;
  return static_cast<ValueMiscFlag>(~static_cast<T>(RHS));
}

enum class MiscFlag : uint8_t {
  NONE = 0x0,
  ShortGrouping = 1 << 0,
  Default = (ShortGrouping)
};

constexpr MiscFlag operator|(MiscFlag LHS, MiscFlag RHS) {
  using T = std::underlying_type_t<MiscFlag>;
  return static_cast<MiscFlag>(static_cast<T>(LHS) | static_cast<T>(RHS));
}

constexpr MiscFlag &operator|=(MiscFlag &LHS, MiscFlag RHS) {
  using T = std::underlying_type_t<MiscFlag>;
  return LHS = LHS | RHS;
}

constexpr MiscFlag operator&(MiscFlag LHS, MiscFlag RHS) {
  using T = std::underlying_type_t<MiscFlag>;
  return static_cast<MiscFlag>(static_cast<T>(LHS) & static_cast<T>(RHS));
}

constexpr MiscFlag &operator&=(MiscFlag &LHS, MiscFlag RHS) {
  using T = std::underlying_type_t<MiscFlag>;
  return LHS = LHS & RHS;
}

constexpr MiscFlag operator~(MiscFlag RHS) {
  using T = std::underlying_type_t<MiscFlag>;
  return static_cast<MiscFlag>(~static_cast<T>(RHS));
}

class OptionBase {
protected:
  uint16_t Occurences = 0;
  OccurrencesFlag OccurF : 3;
  ValueExpected ExpectedF : 2;

  OptionBase(OccurrencesFlag Occur, ValueExpected Ex)
      : Occurences(0), OccurF(Occur), ExpectedF(Ex) {}

public:
  [[nodiscard]] virtual bool isHidden() const { return false; }
  [[nodiscard]] virtual TypeOption getTypeOption() const {
    return TypeOption::Normal;
  }
  [[nodiscard]] OccurrencesFlag getOccurences() const { return OccurF; }
  [[nodiscard]] ValueExpected getExpected() const { return ExpectedF; }

  [[nodiscard]] virtual std::string_view getLongOpt() const { return ""; }
  [[nodiscard]] virtual char8_t getShortOpt() const { return '\0'; }

  [[nodiscard]] virtual std::string_view getDesc() const { return ""; }
  [[nodiscard]] virtual std::string_view getValueDesc() const { return ""; }

  [[nodiscard]] virtual MiscFlag getMisc() const { return MiscFlag::NONE; }
};

template <typename TParser> class OptionBaseBuild : public OptionBase {
protected:
  TParser TypeParser;
  std::string_view Desc;
  std::string_view ValueDesc;

public:
  using OptionBase::OptionBase;

  [[nodiscard]] TypeOption getTypeOption() const override {
    return TypeOption::Normal;
  }

  [[nodiscard]] std::string_view getDesc() const override { return Desc; }
  [[nodiscard]] std::string_view getValueDesc() const override {
    return ValueDesc;
  }
};

template <typename T> struct Applier {
  template <typename O> static void apply(O *Opt, T &&V) { V.apply(Opt); }
};

template <size_t N> struct Applier<char[N]> {
  template <typename O> static void apply(O *Opt, std::string_view Str) {
    Opt->setLongOpt(Str);
  }
};

template <> struct Applier<char> {
  template <typename O> static void apply(O *Opt, char S) {
    Opt->setShortOpt(static_cast<char8_t>(S));
  }
};

template <> struct Applier<char8_t> {
  template <typename O> static void apply(O *Opt, char8_t S) {
    Opt->setShortOpt(S);
  }
};

template <> struct Applier<OccurrencesFlag> {
  template <typename O> static void apply(O *Opt, OccurrencesFlag Occ) {
    Opt->getOccurencesFlag(Occ);
  }
};

template <> struct Applier<ValueExpected> {
  template <typename O> static void apply(O *Opt, ValueExpected V) {
    Opt->setExpected(V);
  }
};

template <> struct Applier<ValueMiscFlag> {
  template <typename O> static void apply(O *Opt, ValueMiscFlag V) {
    Opt->addValueMiscFlag(V);
  }
};

template <> struct Applier<MiscFlag> {
  template <typename O> static void apply(O *Opt, MiscFlag V) {
    Opt->addMiscFlag(V);
  }
};

template <typename T, typename TParser>
class arg : public OptionBaseBuild<TParser> {
private:
  T Value;
  std::string_view LongOpt;
  char8_t ShortOpt;
  MiscFlag Misc = MiscFlag::Default;

  bool Hidden : 1;

public:
  template <typename... Ts> explicit arg(Ts &&...Args) {
    (Applier<Ts>::apply(this, std::forward<Ts>(Args)), ...);
  }

  void removeMisc(MiscFlag F) { Misc &= ~F; }
  void addMisc(MiscFlag F) { Misc |= F; }

  [[nodiscard]] bool isHidden() const override { return Hidden; }
  [[nodiscard]] TypeOption getTypeOption() const override {
    return TypeOption::Normal;
  }

  [[nodiscard]] std::string_view getLongOpt() const override { return LongOpt; }
  [[nodiscard]] char8_t getShortOpt() const override { return ShortOpt; }
  [[nodiscard]] MiscFlag getMisc() const { return Misc; }

  operator T() { return Value; }
  T operator->() { return Value; }

  operator T() const { return Value; }
  T operator->() const { return Value; }
};

template <typename T, typename TParser>
  requires std::is_class_v<T>
class arg<T, TParser> : public T, public OptionBaseBuild<TParser> {
private:
  T Value;
  std::string_view LongOpt;
  char8_t ShortOpt;
  MiscFlag Misc = MiscFlag::Default;

  bool Hidden : 1;

public:
  template <typename... Ts> explicit arg(Ts &&...Args) {
    (Applier<Ts>::apply(this, std::forward<Ts>(Args)), ...);
  }

  void removeMisc(MiscFlag F) { Misc &= ~F; }
  void addMisc(MiscFlag F) { Misc |= F; }

  [[nodiscard]] bool isHidden() const override { return Hidden; }
  [[nodiscard]] TypeOption getTypeOption() const override {
    return TypeOption::Normal;
  }

  [[nodiscard]] std::string_view getLongOpt() const override { return LongOpt; }
  [[nodiscard]] char8_t getShortOpt() const override { return ShortOpt; }
  [[nodiscard]] MiscFlag getMisc() const { return Misc; }
};

template <typename T, typename TParser>
class positional : public OptionBaseBuild<TParser> {
private:
  T Value;

public:
  template <typename... Ts> explicit positional(Ts &&...Args) {
    (Applier<Ts>::apply(this, std::forward<Ts>(Args)), ...);
  }

  [[nodiscard]] bool isHidden() const override { return false; }
  [[nodiscard]] TypeOption getTypeOption() const override {
    return TypeOption::Positional;
  }

  operator T() { return Value; }
  T operator->() { return Value; }

  operator T() const { return Value; }
  T operator->() const { return Value; }
};

template <typename T, typename TParser>
  requires std::is_class_v<T>
class positional<T, TParser> : public T, public OptionBaseBuild<TParser> {
private:
  T Value;

public:
  template <typename... Ts> explicit positional(Ts &&...Args) {
    (Applier<Ts>::apply(this, std::forward<Ts>(Args)), ...);
  }

  [[nodiscard]] bool isHidden() const override { return false; }
  [[nodiscard]] TypeOption getTypeOption() const override {
    return TypeOption::Positional;
  }
};
} // namespace clihehe

#endif
