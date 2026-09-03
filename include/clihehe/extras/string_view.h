//===- StringRef.h - Constant String Reference Wrapper ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//============================================================================//
// clihehe/extras/string_view.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Constant string reference wrapper for C++
/// This is llvm::StringRef
///
//============================================================================//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-Exception
//
//============================================================================//

#ifndef NAO_CLIHEHE_EXTRAS_STRINGVIEW_H
#define NAO_CLIHEHE_EXTRAS_STRINGVIEW_H

#include "clihehe/macros.h"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <functional>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#ifdef NAO_CLIHH_FMT_FORMAT
#  include "fmt/format.h"
#endif

// NOLINTBEGIN(readability-container-contains,readability-braces-around-statements,readability-implicit-bool-conversion,modernize-avoid-c-style-cast)
namespace clihehe::nao {
template <typename Fn> class function_ref;
template <typename Ret, typename... Params> class function_ref<Ret(Params...)> {
  Ret (*callback)(intptr_t callable, Params... params) = nullptr;
  intptr_t callable;

  template <typename Callable>
  static Ret callback_fn(intptr_t callable, Params... params) {
    return (*reinterpret_cast<Callable *>(callable))(
        std::forward<Params>(params)...);
  }

public:
  function_ref() = default;

  template <typename Callable>
  function_ref(
      Callable &&callable,
      // This is not the copy-constructor.
      std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Callable>,
                                       function_ref>> * = nullptr,
      // Functor must be callable and return a suitable type.
      std::enable_if_t<std::is_void_v<Ret> ||
                       std::is_convertible_v<decltype(std::declval<Callable>()(
                                                 std::declval<Params>()...)),
                                             Ret>> * = nullptr)
      : callback(callback_fn<std::remove_reference_t<Callable>>),
        callable(reinterpret_cast<intptr_t>(&callable)) {}

  Ret operator()(Params... params) const {
    return callback(callable, std::forward<Params>(params)...);
  }

  explicit operator bool() const { return callback; }

  bool operator==(const function_ref<Ret(Params...)> &Other) const {
    return callable == Other.callable;
  }
};

inline char toLower(char x) {
  if ('A' <= x && x <= 'Z')
    return x - 'A' + 'a';
  return x;
}

inline char toUpper(char x) {
  if ('a' <= x && x <= 'z')
    return x - 'a' + 'A';
  return x;
}

inline int ascii_strncasecmp(std::string_view LHS, std::string_view RHS) {
  assert(LHS.size() == RHS.size() &&
         "2 these string_view don't have a same size");
  for (size_t I = 0; I < RHS.size(); I++) {
    auto LC = LHS[I];
    auto RC = RHS[I];

    unsigned char LHC = toLower(LC);
    unsigned char RHC = toLower(RC);
    if (LHC != RHC)
      return LHC < RHC ? -1 : 1;
  }
  return 0;
}

class StringRef {
public:
  static constexpr size_t npos = ~(size_t)0;

  using iterator = const char *;
  using const_iterator = const char *;
  using size_type = size_t;
  using value_type = char;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr StringRef() = default;
  constexpr StringRef(std::nullptr_t) = delete;

  constexpr StringRef(std::string_view S) : Data_(S.data()), Size_(S.size()) {}
  constexpr StringRef(const char *S)
      : StringRef(S ? std::string_view(S) : std::string_view()) {}
  constexpr StringRef(std::string const &S)
      : Data_(S.data()), Size_(S.size()) {}
  constexpr StringRef(const char *S, size_t Size) : Data_(S), Size_(Size) {}

  [[nodiscard]] constexpr const char *data() const { return Data_; }
  [[nodiscard]] constexpr size_t size() const { return Size_; }

  [[nodiscard]] constexpr bool empty() const { return size() == 0; }

  [[nodiscard]] char front() const {
    assert(!empty());
    return *data();
  }
  [[nodiscard]] char back() const {
    assert(!empty());
    return data()[size() - 1];
  }
  [[nodiscard]] char operator[](size_t N) const {
    assert(N < size() && "Invalid index!");
    return data()[N];
  }

  template <typename T>
    requires std::same_as<T, std::string>
  StringRef operator=(T &&S) = delete;

  constexpr operator std::string_view() const { return {data(), size()}; }

  [[nodiscard]] std::string str() const { return {data(), size()}; }

  [[nodiscard]] iterator begin() const { return data(); }
  [[nodiscard]] iterator end() const { return data() + size(); }

  [[nodiscard]] reverse_iterator rbegin() const {
    return std::make_reverse_iterator(end());
  }
  [[nodiscard]] reverse_iterator rend() const {
    return std::make_reverse_iterator(begin());
  }

  // Convert the given ASCII string to lowercase.
  [[nodiscard]] std::string lower() const {
    auto V = std::ranges::views::transform(*this, toLower);
    return {V.begin(), V.end()};
  }

  /// Convert the given ASCII string to uppercase.
  [[nodiscard]] std::string upper() const {
    auto V = std::ranges::views::transform(*this, toUpper);
    return {V.begin(), V.end()};
  }

  // Substr
  [[nodiscard]] StringRef substr(size_t Start, size_t N = npos) const {
    Start = std::min(Start, size());
    return {data() + Start, std::min(N, size() - Start)};
  }

  [[nodiscard]] StringRef drop_front(size_t N = 1) const {
    assert(size() >= N && "Dropping more elements than exist");
    return substr(N);
  }

  [[nodiscard]] StringRef drop_back(size_t N = 1) const {
    assert(size() >= N && "Dropping more elements than exists");
    return substr(0, size() - N);
  }

  [[nodiscard]] StringRef take_front(size_t N = 1) const {
    if (N >= size())
      return *this;
    return drop_back(size() - N);
  }

  [[nodiscard]] StringRef take_back(size_t N = 1) const {
    if (N >= size())
      return *this;
    return drop_front(size() - N);
  }

  [[nodiscard]] StringRef slice(size_t Start, size_t End) const {
    Start = std::min(Start, size());
    End = std::clamp(End, Start, size());
    return {data() + Start, End - Start};
  }

  [[nodiscard]] std::pair<StringRef, StringRef> split(StringRef Sep) const {
    size_t Idx = find(Sep);
    if (Idx == npos)
      return {*this, StringRef()};
    return {slice(0, Idx), substr(Idx + Sep.size())};
  }

  [[nodiscard]] std::pair<StringRef, StringRef> split(char Sep) const {
    return split(StringRef(&Sep, 1));
  }

  [[nodiscard]] std::pair<StringRef, StringRef> rsplit(StringRef Sep) const {
    size_t Idx = rfind(Sep);
    if (Idx == npos)
      return {*this, StringRef{}};
    return {slice(0, Idx), substr(Idx + Sep.size())};
  }

  [[nodiscard]] std::pair<StringRef, StringRef> rsplit(char Sep) const {
    return rsplit(StringRef(&Sep, 1));
  }

  [[nodiscard]] StringRef ltrim(char Char) const {
    return drop_front(std::min(size(), find_first_not_of(Char)));
  }
  [[nodiscard]] StringRef ltrim(StringRef Chars = " \t\n\v\f\r") const {
    return drop_front(std::min(size(), find_first_not_of(Chars)));
  }

  [[nodiscard]] StringRef rtrim(char Char) const {
    return drop_back(size() - std::min(size(), find_last_not_of(Char) + 1));
  }
  [[nodiscard]] StringRef rtrim(StringRef Chars = " \t\n\v\f\r") const {
    return drop_back(size() - std::min(size(), find_last_not_of(Chars) + 1));
  }

  [[nodiscard]] StringRef trim(char Char) const {
    return ltrim(Char).rtrim(Char);
  }
  [[nodiscard]] StringRef trim(StringRef Chars = " \t\n\v\f\r") const {
    return ltrim(Chars).rtrim(Chars);
  }

  // Compare

  [[nodiscard]] int compare(StringRef RHS) const {
    if (int Res =
            compareMemory(data(), RHS.data(), std::min(size(), RHS.size())))
      return Res < 0 ? -1 : 1;

    if (size() == RHS.size())
      return 0;
    return size() < RHS.size() ? -1 : 1;
  }

  [[nodiscard]] int compare_insensitive(StringRef RHS) const {
    size_t Min = std::min(size(), RHS.size());
    if (int Res = ascii_strncasecmp(take_front(Min), RHS.take_front(Min)))
      return Res;
    if (size() == RHS.size())
      return 0;
    return size() < RHS.size() ? -1 : 1;
  }

  [[nodiscard]] bool equals_insensitive(StringRef RHS) const {
    return size() != RHS.size() && compare_insensitive(RHS) == 0;
  }

  // String predicates

  [[nodiscard]] bool starts_with(StringRef Prefix) const {
    return size() >= Prefix.size() &&
           compareMemory(data(), Prefix.data(), Prefix.size()) == 0;
  }

  [[nodiscard]] bool starts_with(char Prefix) const {
    return !empty() && front() == Prefix;
  }

  [[nodiscard]] bool starts_with_insensitive(StringRef Prefix) const {
    return size() >= Prefix.size() &&
           ascii_strncasecmp(take_front(Prefix.size()), Prefix) == 0;
  }

  [[nodiscard]] bool ends_with(StringRef Suffix) const {
    return size() >= Suffix.size() &&
           compareMemory(end() - Suffix.size(), Suffix.data(), Suffix.size()) ==
               0;
  }

  [[nodiscard]] bool ends_with(char Suffix) const {
    return !empty() && back() == Suffix;
  }

  [[nodiscard]] bool ends_with_insensitive(StringRef Suffix) const {
    return size() >= Suffix.size() &&
           ascii_strncasecmp(take_back(Suffix.size()), Suffix) == 0;
  }

  // String searching

  [[nodiscard]] size_t find(char Char, size_t From = 0) const {
    return std::string_view{*this}.find(Char, From);
  }

  // NOLINTNEXTLINE
  [[nodiscard]] size_t find(StringRef Chars, size_t From = 0) const {
    if (From > size())
      return npos;

    const char *Start = data() + From;
    size_t Size = size() - From;

    const char *Needle = Chars.data();
    size_t N = Chars.size();
    if (N == 0)
      return From;
    if (Size < N)
      return npos;
    if (N == 1) {
      const char *Ptr = (const char *)::memchr(Start, Needle[0], Size);
      return Ptr == nullptr ? npos : Ptr - data();
    }

    const char *Stop = Start + (Size - N + 1);

    if (N == 2) {
      do {
        if (std::memcmp(Start, Needle, 2) == 0)
          return Start - data();
        ++Start;
      } while (Start < Stop);
      return npos;
    }

    if (Size < 16 || N > 255) {
      do {
        if (std::memcmp(Start, Needle, N) == 0)
          return Start - data();
        ++Start;
      } while (Start < Stop);
      return npos;
    }

    // Build the bad char heuristic table, with uint8_t to reduce cache
    // thrashing.
    uint8_t BadCharSkip[256];
    std::memset(BadCharSkip, N, 256);
    for (unsigned i = 0; i != N - 1; ++i)
      BadCharSkip[(uint8_t)Chars[i]] = N - 1 - i;

    do {
      uint8_t Last = Start[N - 1];
      if (Last == (uint8_t)Needle[N - 1]) [[unlikely]]
        if (std::memcmp(Start, Needle, N - 1) == 0)
          return Start - data();

      Start += BadCharSkip[Last];
    } while (Start < Stop);

    return npos;
  }

  [[nodiscard]] size_t find_if(function_ref<bool(char)> F,
                               size_t From = 0) const {
    StringRef S = drop_front(From);
    while (!S.empty()) {
      if (F(S.front()))
        return size() - S.size();
      S = S.drop_front();
    }
    return npos;
  }

  [[nodiscard]] size_t find_if_not(function_ref<bool(char)> F,
                                   size_t From = 0) const {
    return find_if([F](char c) { return !F(c); }, From);
  }

  [[nodiscard]] size_t find_insensitive(char C, size_t From = 0) const {
    char L = toLower(C);
    return find_if([L](char D) { return toLower(D) == L; }, From);
  }

  [[nodiscard]] size_t find_insensitive(StringRef Str, size_t From = 0) const {
    StringRef This = substr(From);
    while (This.size() >= Str.size()) {
      if (This.starts_with_insensitive(Str))
        return From;
      This = This.drop_front();
      ++From;
    }
    return npos;
  }

  [[nodiscard]] size_t find_first_of(char C, size_t From = 0) const {
    return find(C, From);
  }

  [[nodiscard]] size_t find_first_of(StringRef Chars, size_t From = 0) const {
    std::bitset<1 << CHAR_BIT> CharBits;
    for (char C : Chars)
      CharBits.set((unsigned char)C);

    for (size_type I = std::min(From, size()), e = size(); I != e; ++I)
      if (CharBits.test((unsigned)data()[I]))
        return I;
    return npos;
  }

  [[nodiscard]] size_t find_first_not_of(char C, size_t From = 0) const {
    return std::string_view{*this}.find_first_not_of(C, From);
  }

  [[nodiscard]] size_t find_first_not_of(StringRef Chars,
                                         size_t From = 0) const {
    std::bitset<1 << CHAR_BIT> CharBits;
    for (char C : Chars) {
      CharBits.set((unsigned char)C);
    }

    for (size_type I = std::min(From, size()), e = size(); I != e; ++I) {
      if (!CharBits.test((unsigned char)data()[I]))
        return I;
    }
    return npos;
  }

  [[nodiscard]] size_t find_last_of(char C, size_t From = npos) const {
    return rfind(C, From);
  }

  [[nodiscard]] size_t find_last_of(StringRef Chars, size_t From = npos) const {
    std::bitset<1 << CHAR_BIT> CharBits;
    for (char C : Chars)
      CharBits.set((unsigned)C);

    for (size_type i = std::min(From, size()), e = -1; i != e; --i)
      if (CharBits.test((unsigned char)data()[i]))
        return i;
    return npos;
  }

  /// find_last_not_of - Find the last character in the string that is not
  /// \arg C, or npos if not found.
  [[nodiscard]] size_type find_last_not_of(char C, size_t From = npos) const {
    for (size_type i = std::min(From, size()) - 1, e = -1; i != e; --i)
      if (data()[i] != C)
        return i;
    return npos;
  }

  /// find_last_not_of - Find the last character in the string that is not in
  /// \arg Chars, or npos if not found.
  ///
  /// Note: O(size() + Chars.size())
  [[nodiscard]] size_type find_last_not_of(StringRef Chars,
                                           size_t From = npos) const {
    std::bitset<1 << CHAR_BIT> CharBits;
    for (char C : Chars)
      CharBits.set((unsigned char)C);

    for (size_type i = std::min(From, size()) - 1, e = -1; i != e; --i)
      if (!CharBits.test((unsigned char)data()[i]))
        return i;
    return npos;
  }

  [[nodiscard]] size_t rfind(char C, size_t From = npos) const {
    size_t I = std::min(From, size());
    while (I) {
      --I;
      if (data()[I] == C)
        return I;
    }

    return npos;
  }

  [[nodiscard]] size_t rfind(StringRef Chars, size_t Pos = npos) const {
    return std::string_view{*this}.rfind(Chars, Pos);
  }

  [[nodiscard]] size_t rfind_insensitive(char C, size_t From = npos) const {
    From = std::min(From, size());
    size_t I = From;
    while (I != 0) {
      --I;
      if (toLower(data()[I]) == toLower(C)) {
        return I;
      }
    }

    return npos;
  }

  [[nodiscard]] size_t rfind_insensitive(StringRef Chars,
                                         size_t From = npos) const {
    size_t N = Chars.size();
    if (N > size())
      return npos;
    for (size_t i = size() - N + 1, e = 0; i != e; --i) {
      if (substr(i, N).equals_insensitive(Chars))
        return i;
    }
    return npos;
  }

  [[nodiscard]] size_t rfind_if(function_ref<bool(char)> F,
                                size_t End = npos) const {
    size_t I = std::min(End, size());
    while (I) {
      --I;
      if (F(data()[I]))
        return I;
    }
    return npos;
  }

  [[nodiscard]] size_t rfind_if_not(function_ref<bool(char)> F,
                                    size_t End = npos) const {
    return rfind_if(std::not_fn(F), End);
  }

  [[nodiscard]] bool contains(StringRef Other) const {
    return find(Other) != npos;
  }

  [[nodiscard]] bool contains(char C) const { return find_first_of(C) != npos; }

  [[nodiscard]] bool contains_insensitive(StringRef Other) const {
    return find_insensitive(Other) != npos;
  }

  [[nodiscard]] bool contains_insensitive(char C) const {
    return find_insensitive(C) != npos;
  }

  // operator

private:
  static int compareMemory(const char *Lhs, const char *Rhs, size_t Length) {
    if (Length == 0)
      return 0;
    return ::memcmp(Lhs, Rhs, Length);
  }

  const char *Data_ = nullptr;
  size_t Size_ = 0;
};

inline bool operator==(StringRef LHS, StringRef RHS) {
  if (LHS.size() != RHS.size())
    return false;
  if (LHS.empty())
    return true;
  return ::memcmp(LHS.data(), RHS.data(), LHS.size()) == 0;
}

inline bool operator!=(StringRef LHS, StringRef RHS) { return !(LHS == RHS); }

inline bool operator<(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) < 0;
}

inline bool operator<=(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) <= 0;
}

inline bool operator>(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) > 0;
}

inline bool operator>=(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) >= 0;
}

inline std::string &operator+=(std::string &buffer, StringRef string) {
  return buffer.append(string.data(), string.size());
}

class StringLiteral : public StringRef {
public:
  template <size_t N>
  constexpr StringLiteral(const char (&Chars)[N]) : StringRef(Chars, N - 1) {}
};
} // namespace clihehe::nao
// NOLINTEND(readability-container-contains,readability-braces-around-statements,readability-implicit-bool-conversion,modernize-avoid-c-style-cast)

namespace std {
template <>
struct formatter<clihehe::nao::StringRef> : std::formatter<std::string_view> {};
} // namespace std

#ifdef FMT_VERSION
namespace fmt {
template <>
struct formatter<clihehe::nao::StringRef> : fmt::formatter<std::string_view> {};
} // namespace fmt
#endif

#endif
