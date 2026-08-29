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

#include "clihehe/extras/ranges.h"
#include <cassert>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <vector>

namespace clihehe {
enum Occur : uint8_t {
  Optional,
  ZeroOrMore,
  SRequired,
  SOneOrMore,
  SConsumeAfter
};
enum ValueExpected : uint8_t { VOptional = 1, VRequired = 2, VDisallowed = 3 };
enum Formatting : uint8_t { NormFor = 0, Positional = 1, CompilerLike = 2 };
enum Misc : uint8_t { Norm = 0 << 1, EatArgs = 1 << 1, SepComma = 2 << 1 };
enum ShortMisc : uint8_t { SNorm = 0b1, SCombining = 1 << 1 };

template <typename Sub>
  requires(!std::same_as<Sub, void> || std::is_reference_v<Sub> ||
           std::is_pointer_v<Sub>)
class CLIParserSubStorage {
private:
  std::vector<Sub *> Subs;

protected:
  CLIParserSubStorage() = default;

public:
  ~CLIParserSubStorage() = default;

  void register_subcommand(Sub *S) {
    assert(S && "Invalid address subcommand");
    assert(!nao::ranges_contains(Subs, S) &&
           "this subcommand already registered");
    Subs.push_back(S);
  }
};

template <typename Opt>
  requires(!std::same_as<Opt, void> || std::is_reference_v<Opt> ||
           std::is_pointer_v<Opt>)

class CLIParserOptStorage {
private:
  std::vector<Opt *> Opts;

protected:
  CLIParserOptStorage() = default;

public:
  ~CLIParserOptStorage() = default;

  void register_opt(Opt *S) {
    assert(S && "Invalid address option");
    assert(!nao::ranges_contains(Opts, S) && "this option already registered");
    Opts.push_back(S);
  }
};

template <typename Cate>
  requires(!std::same_as<Cate, void> || std::is_reference_v<Cate> ||
           std::is_pointer_v<Cate>)
class CLIParserCateStorage {
private:
  std::vector<Cate *> Opts;

protected:
  CLIParserCateStorage() = default;

public:
  ~CLIParserCateStorage() = default;

  void register_cate(Cate *S) {
    assert(S && "Invalid address category");
    assert(!nao::ranges_contains(Opts, S) &&
           "this category already registered");
    Opts.push_back(S);
  }
};

template <typename OptionType, typename OptCateType, typename Sub>
class CLIParser : public CLIParserOptStorage<OptionType>,
                  public CLIParserCateStorage<OptCateType>,
                  public CLIParserSubStorage<Sub> {
  using sub_stor = CLIParserSubStorage<Sub>;
  using opt_stor = CLIParserOptStorage<Sub>;
  using cate_stor = CLIParserCateStorage<Sub>;

private:
  std::vector<OptionType *> Opts;
  std::vector<OptCateType *> Cates;

public:
  CLIParser() = default;
  ~CLIParser() = default;
};

class OptionBase {
private:
  uint8_t OccurValue : 3;
  uint8_t ExpectedValue : 2;
  uint8_t FormatValue : 2;
  uint8_t MiscValue : 2;
  uint8_t SMiscValue : 1;
  bool Show : 1;

protected:
  OptionBase()
      : OccurValue(Optional), ExpectedValue(VOptional), FormatValue(NormFor),
        MiscValue(SNorm), SMiscValue(SNorm), Show(true) {}
  virtual ~OptionBase() = default;

public:
  std::string_view Long;
  char8_t Short;

  [[nodiscard]] bool hasLongOpt() const { return !Long.empty(); }
  [[nodiscard]] bool hasShortOpt() const { return Short != 0x00; }

  void changeOccur(Occur O) { this->OccurValue = O; }
  void changeExpected(ValueExpected Ex) { ExpectedValue = Ex; }
  void changeFormat(Formatting For) { this->FormatValue = For; }
  void addShortMisc(ShortMisc S) { this->SMiscValue |= S; }
  void addMisc(Misc S) { this->MiscValue |= S; }
  void hideThisOption() { Show = true; }
};

class Sub {
public:
  std::string_view const Name;
  std::string_view const Desc;

  std::vector<OptionBase *> Options;
  std::vector<OptionBase *> Position;

  Sub(std::string_view Name, std::string_view Desc) : Name(Name), Desc(Desc) {}
  Sub(CLIParserSubStorage<Sub> &P, std::string_view Name, std::string_view Desc)
      : Sub(Name, Desc) {
    P.register_subcommand(this);
  }
};

class OptionCate {
public:
  std::string_view const Name;
  std::string_view const Desc;

  OptionCate(std::string_view Name, std::string_view Desc)
      : Name(Name), Desc(Desc) {}
  OptionCate(CLIParserCateStorage<OptionCate> &P, std::string_view N,
             std::string_view D)
      : OptionCate(N, D) {
    P.register_cate(this);
  }
};

template <typename Applier>
concept ValidOptionApplier =
    requires(Applier &&A, OptionBase *B) { A.apply(B); };

template <class ParserT>
concept ValidParserForOpt = requires(ParserT A) { std::is_class_v<ParserT>; };

template <typename T> class opt : public OptionBase {
private:
  T Val;

public:
  template <typename... Appliers>
    requires(ValidOptionApplier<Appliers> && ...)
  explicit opt(Appliers &&...Args) {
    (std::forward<Appliers>(Args).apply(this), ...);
  }
  ~opt() override = default;

  T *operator->() { return &Val; }
  T *operator->() const { return &Val; }

  operator T() const { return Val; }
  operator T() { return Val; }
};

template <typename T>
  requires std::is_class_v<T>
class opt<T> : public T, public OptionBase {
public:
};

} // namespace clihehe

#endif
