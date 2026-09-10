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

#include "clihehe/extras/string_view.h"
#include "clihehe/fmt.h"
#include "macros.h"
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
  [[nodiscard]] uint16_t getNumOccur() const { return Occurences; }
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

class Parser {
public:
  explicit Parser(std::string_view ProcessName, std::string_view Desc = "")
      : ProgramName(ProcessName), ProgramDesc(Desc) {}

  static bool RequiresValue(const OptionBase *O) {
    return O->getOccurences() == OccurrencesFlag::Required ||
           O->getOccurences() == OccurrencesFlag::OneOrMore;
  }

  static bool EatsUnboundedNumberOfValues(const OptionBase *O) {
    return O->getOccurences() == OccurrencesFlag::ZeroOrMore ||
           O->getOccurences() == OccurrencesFlag::OneOrMore;
  }

  /// ProvideOption - For Value, this differentiates between an empty
  /// value ("")
  /// and a null value (StringRef()).  The later is accepted for arguments
  /// that don't allow a value (-foo) the former is rejected (-foo=).
  static bool ProvideOption(OptionBase *Handler, nao::StringRef ArgName,
                            nao::StringRef Value, int argc,
                            const char *const *argv, int &i) {
    // Is this a multi-argument option?
    unsigned NumAdditionalVals = Handler->getNumAdditionalVals();

    // Enforce value requirements
    switch (Handler->getExpected()) {
    case ValueExpected::Required:
      if (!Value.data()) { // No value specified?
        // If no other argument or the option only supports prefix form, we
        // cannot look at the next argument.
        if (i + 1 >= argc || Handler->getFormattingFlag() == cl::AlwaysPrefix)
          return Handler->error("requires a value!");
        // Steal the next argument, like for '-o filename'
        assert(argv && "null check");
        Value = StringRef(argv[++i]);
      }
      break;
    case ValueExpected::Disallowed:
      if (NumAdditionalVals > 0)
        return Handler->error("multi-valued option specified"
                              " with ValueDisallowed modifier!");

      if (Value.data())
        return Handler->error("does not allow a value! '" + Twine(Value) +
                              "' specified.");
      break;
    case ValueExpected::Optional:
      break;
    }

    // If this isn't a multi-arg option, just run the handler.
    if (NumAdditionalVals == 0)
      return CommaSeparateAndAddOccurrence(Handler, i, ArgName, Value);

    // If it is, run the handle several times.
    bool MultiArg = false;

    if (Value.data()) {
      if (CommaSeparateAndAddOccurrence(Handler, i, ArgName, Value, MultiArg))
        return true;
      --NumAdditionalVals;
      MultiArg = true;
    }

    while (NumAdditionalVals > 0) {
      if (i + 1 >= argc)
        return Handler->error("not enough values!");
      assert(argv && "null check");
      Value = nao::StringRef(argv[++i]);

      if (CommaSeparateAndAddOccurrence(Handler, i, ArgName, Value, MultiArg))
        return true;
      MultiArg = true;
      --NumAdditionalVals;
    }
    return false;
  }

  static bool ProvidePositionalOption(OptionBase *Handler, nao::StringRef Arg,
                                      int i) {
    int Dummy = i;
    return ProvideOption(Handler, Handler->getLongOpt(), Arg, 0, nullptr,
                         Dummy);
  }

#ifdef NAO_CLIHH_PRT_NS
  template <typename... Ts>
  bool ErrorPrint(OptionBase *Opt, NAO_CLIHH_PRT_NS::format_string<Ts...> Fmt,
                  Ts &&...Args) {
    if (Opt->getTypeOption() == TypeOption::Positional)
      NAO_CLIHH_PRT_NS::print(stderr, "{}", Opt->getDesc());
    else
      NAO_CLIHH_PRT_NS::print(stderr, "{}: for the {}", ProgramName,
                              Opt->getLongOpt());

    NAO_CLIHH_PRT_NS::print(stderr, " option: ");
    NAO_CLIHH_PRT_NS::print(Fmt, std::forward<Ts>(Args)...);

    return true;
  }
#endif

  bool parseArgs(int argc, char const *const *argv) {
    std::span<char const *const> Args{argv, static_cast<size_t>(argc)};

    bool ErrorParsing = false;

    // Check out the positional arguments to collect information about them.
    unsigned NumPositionalRequired = 0;

    // Determine whether or not there are an unlimited number of positionals
    bool HasUnlimitedPositionals = false;

    auto &ConsumeAfterOpt = this->ConsumeAfterOpt;
    auto &PositionalOpts = this->PositionalOpts;
    auto &SinkOpts = this->SinkOpts;
    auto &OptionsMap = this->OptionsMap;

    if (ConsumeAfterOpt) {
      assert(PositionalOpts.size() > 0 &&
             "Cannot specify cl::ConsumeAfter without a positional argument!");
    }

    if (!PositionalOpts.empty()) {

      // Calculate how many positional values are _required_.
      bool UnboundedFound = false;
      for (size_t i = 0, e = PositionalOpts.size(); i != e; ++i) {
        OptionBase *Opt = PositionalOpts[i];
        if (RequiresValue(Opt))
          ++NumPositionalRequired;
        else if (ConsumeAfterOpt) {
          // ConsumeAfter cannot be combined with "optional" positional options
          // unless there is only one positional argument...
          if (PositionalOpts.size() > 1) {
            this->ErrorPrint(
                Opt, "error - this positional option will never be matched, "
                     "because it does not Require a value, and a "
                     "cl::ConsumeAfter option is active!");
            ErrorParsing = true;
          }
        } else if (UnboundedFound &&
                   (Opt->getLongOpt().empty() && Opt->getShortOpt() != '\0')) {
          // This option does not "require" a value...  Make sure this option is
          // not specified after an option that eats all extra arguments, or
          // this one will never get any!
          //
          this->ErrorPrint(Opt, "error - option can never match, because "
                                "another positional argument will match an "
                                "unbounded number of values, and this option"
                                " does not require a value!");
          NAO_CLIHH_PRT_NS::print(
              ": CommandLine Error: Option '{}' is all messed up!\n",
              Opt->getLongOpt());
          NAO_CLIHH_PRT_NS::print("{}", PositionalOpts.size());
          ErrorParsing = true;
        }
        UnboundedFound |= EatsUnboundedNumberOfValues(Opt);
      }
      HasUnlimitedPositionals = UnboundedFound || ConsumeAfterOpt;
    }

    // PositionalVals - A vector of "positional" arguments we accumulate into
    // the process at the end.
    //
    std::vector<std::pair<nao::StringRef, unsigned>> PositionalVals;

    // If the program has named positional arguments, and the name has been run
    // across, keep track of which positional argument was named.  Otherwise put
    // the positional args into the PositionalVals list...
    OptionBase *ActivePositionalArg = nullptr;

    // Loop over all of the arguments... processing them.
    bool DashDashFound = false; // Have we read '--'?
    for (int i = 1; i < argc; ++i) {
      OptionBase *Handler = nullptr;
      std::string NearestHandlerString;
      nao::StringRef Value;
      nao::StringRef ArgName = "";
      bool HaveDoubleDash = false;

      // Check to see if this is a positional argument.  This argument is
      // considered to be positional if it doesn't start with '-', if it is "-"
      // itself, or if we have seen "--" already.
      //
      if (argv[i][0] != '-' || argv[i][1] == 0 || DashDashFound) {
        // Positional argument!
        if (ActivePositionalArg) {
          ProvidePositionalOption(ActivePositionalArg, nao::StringRef(argv[i]),
                                  i);
          continue; // We are done!
        }

        if (!PositionalOpts.empty()) {
          PositionalVals.push_back(std::make_pair(StringRef(argv[i]), i));

          // All of the positional arguments have been fulfulled, give the rest
          // to the consume after option... if it's specified...
          //
          if (PositionalVals.size() >= NumPositionalRequired &&
              ConsumeAfterOpt) {
            for (++i; i < argc; ++i)
              PositionalVals.push_back(std::make_pair(StringRef(argv[i]), i));
            break; // Handle outside of the argument processing loop...
          }

          // Delay processing positional arguments until the end...
          continue;
        }
      } else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == 0 &&
                 !DashDashFound) {
        DashDashFound = true; // This is the mythical "--"?
        continue;             // Don't try to process it as an argument itself.
      } else if (ActivePositionalArg &&
                 (ActivePositionalArg->getMiscFlags() & PositionalEatsArgs)) {
        // If there is a positional argument eating options, check to see if
        // this option is another positional argument.  If so, treat it as an
        // argument, otherwise feed it to the eating positional.
        ArgName = StringRef(argv[i] + 1);
        // Eat second dash.
        if (ArgName.consume_front("-"))
          HaveDoubleDash = true;

        Handler = LookupLongOption(*ChosenSubCommand, ArgName, Value,
                                   LongOptionsUseDoubleDash, HaveDoubleDash);
        if (!Handler || Handler->getFormattingFlag() != cl::Positional) {
          ProvidePositionalOption(ActivePositionalArg, StringRef(argv[i]), i);
          continue; // We are done!
        }
      } else { // We start with a '-', must be an argument.
        ArgName = StringRef(argv[i] + 1);
        // Eat second dash.
        if (ArgName.consume_front("-"))
          HaveDoubleDash = true;

        Handler = LookupLongOption(*ChosenSubCommand, ArgName, Value,
                                   LongOptionsUseDoubleDash, HaveDoubleDash);

        // If Handler is not found in a specialized subcommand, look up handler
        // in the top-level subcommand.
        // cl::opt without cl::sub belongs to top-level subcommand.
        if (!Handler && ChosenSubCommand != &SubCommand::getTopLevel())
          Handler = LookupLongOption(SubCommand::getTopLevel(), ArgName, Value,
                                     LongOptionsUseDoubleDash, HaveDoubleDash);

        // Check to see if this "option" is really a prefixed or grouped
        // argument.
        if (!Handler && !(LongOptionsUseDoubleDash && HaveDoubleDash))
          Handler = HandlePrefixedOrGroupedOption(ArgName, Value, ErrorParsing,
                                                  OptionsMap);

        // Otherwise, look for the closest available option to report to the
        // user in the upcoming error.
        if (!Handler && SinkOpts.empty())
          LookupNearestOption(ArgName, OptionsMap, NearestHandlerString);
      }

      if (!Handler) {
        if (!SinkOpts.empty()) {
          for (Option *SinkOpt : SinkOpts)
            SinkOpt->addOccurrence(i, "", StringRef(argv[i]));
          continue;
        }

        auto ReportUnknownArgument = [&](bool IsArg,
                                         StringRef NearestArgumentName) {
          *Errs << ProgramName << ": Unknown "
                << (IsArg ? "command line argument" : "subcommand") << " '"
                << argv[i] << "'.  Try: '" << argv[0] << " --help'\n";

          if (NearestArgumentName.empty())
            return;

          *Errs << ProgramName << ": Did you mean '";
          if (IsArg)
            *Errs << PrintArg(NearestArgumentName, 0);
          else
            *Errs << NearestArgumentName;
          *Errs << "'?\n";
        };

        if (i > 1 || !MaybeNamedSubCommand)
          ReportUnknownArgument(/*IsArg=*/true, NearestHandlerString);
        else
          ReportUnknownArgument(/*IsArg=*/false, NearestSubCommandString);

        ErrorParsing = true;
        continue;
      }

      // If this is a named positional argument, just remember that it is the
      // active one...
      if (Handler->getFormattingFlag() == cl::Positional) {
        if ((Handler->getMiscFlags() & PositionalEatsArgs) && !Value.empty()) {
          Handler->error(
              "This argument does not take a value.\n"
              "\tInstead, it consumes any positional arguments until "
              "the next recognized option.",
              *Errs);
          ErrorParsing = true;
        }
        ActivePositionalArg = Handler;
      } else
        ErrorParsing |= ProvideOption(Handler, ArgName, Value, argc, argv, i);
    }

    // Check and handle positional arguments now...
    if (NumPositionalRequired > PositionalVals.size()) {
      *Errs << ProgramName
            << ": Not enough positional command line arguments specified!\n"
            << "Must specify at least " << NumPositionalRequired
            << " positional argument" << (NumPositionalRequired > 1 ? "s" : "")
            << ": See: " << argv[0] << " --help\n";

      ErrorParsing = true;
    } else if (!HasUnlimitedPositionals &&
               PositionalVals.size() > PositionalOpts.size()) {
      *Errs << ProgramName << ": Too many positional arguments specified!\n"
            << "Can specify at most " << PositionalOpts.size()
            << " positional arguments: See: " << argv[0] << " --help\n";
      ErrorParsing = true;

    } else if (!ConsumeAfterOpt) {
      // Positional args have already been handled if ConsumeAfter is specified.
      unsigned ValNo = 0,
               NumVals = static_cast<unsigned>(PositionalVals.size());
      for (Option *Opt : PositionalOpts) {
        if (RequiresValue(Opt)) {
          ProvidePositionalOption(Opt, PositionalVals[ValNo].first,
                                  PositionalVals[ValNo].second);
          ValNo++;
          --NumPositionalRequired; // We fulfilled our duty...
        }

        // If we _can_ give this option more arguments, do so now, as long as we
        // do not give it values that others need.  'Done' controls whether the
        // option even _WANTS_ any more.
        //
        bool Done = Opt->getNumOccurrencesFlag() == cl::Required;
        while (NumVals - ValNo > NumPositionalRequired && !Done) {
          switch (Opt->getNumOccurrencesFlag()) {
          case cl::Optional:
            Done = true; // Optional arguments want _at most_ one value
            [[fallthrough]];
          case cl::ZeroOrMore: // Zero or more will take all they can get...
          case cl::OneOrMore:  // One or more will take all they can get...
            ProvidePositionalOption(Opt, PositionalVals[ValNo].first,
                                    PositionalVals[ValNo].second);
            ValNo++;
            break;
          default:
            llvm_unreachable(
                "Internal error, unexpected NumOccurrences flag in "
                "positional argument processing!");
          }
        }
      }
    } else {
      assert(ConsumeAfterOpt && NumPositionalRequired <= PositionalVals.size());
      unsigned ValNo = 0;
      for (Option *Opt : PositionalOpts)
        if (RequiresValue(Opt)) {
          ErrorParsing |= ProvidePositionalOption(
              Opt, PositionalVals[ValNo].first, PositionalVals[ValNo].second);
          ValNo++;
        }

      // Handle the case where there is just one positional option, and it's
      // optional.  In this case, we want to give JUST THE FIRST option to the
      // positional option and keep the rest for the consume after.  The above
      // loop would have assigned no values to positional options in this case.
      //
      if (PositionalOpts.size() == 1 && ValNo == 0 && !PositionalVals.empty()) {
        ErrorParsing |= ProvidePositionalOption(PositionalOpts[0],
                                                PositionalVals[ValNo].first,
                                                PositionalVals[ValNo].second);
        ValNo++;
      }

      // Handle over all of the rest of the arguments to the
      // cl::ConsumeAfter command line option...
      for (; ValNo != PositionalVals.size(); ++ValNo)
        ErrorParsing |= ProvidePositionalOption(ConsumeAfterOpt,
                                                PositionalVals[ValNo].first,
                                                PositionalVals[ValNo].second);
    }

    // Loop over args and make sure all required args are specified!
    for (const auto &Opt : OptionsMap) {
      switch (Opt.second->getNumOccurrencesFlag()) {
      case Required:
      case OneOrMore:
        if (Opt.second->getNumOccurrences() == 0) {
          Opt.second->error("must be specified at least once!");
          ErrorParsing = true;
        }
        [[fallthrough]];
      default:
        break;
      }
    }

    // Now that we know if -debug is specified, we can use it.
    // Note that if ReadResponseFiles == true, this must be done before the
    // memory allocated for the expanded command line is free()d below.
    LLVM_DEBUG(dbgs() << "Args: ";
               for (int i = 0; i < argc; ++i) dbgs() << argv[i] << ' ';
               dbgs() << '\n';);

    // Free all of the memory allocated to the map.  Command line options may
    // only be processed once!
    MoreHelp.clear();

    // If we had an error processing our arguments, don't let the program
    // execute
    if (ErrorParsing) {
      if (!IgnoreErrors)
        exit(1);
      return false;
    }
    return true;
  }

private:
  std::string_view ProgramName;
  std::string_view ProgramDesc;

  std::vector<OptionBase *> PositionalOpts;
  std::vector<OptionBase *> SinkOpts;
  std::unordered_map<nao::StringRef, OptionBase *> OptionsMap;
  OptionBase *ConsumeAfterOpt = nullptr;
};
} // namespace clihehe

#endif
