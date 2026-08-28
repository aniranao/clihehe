//============================================================================//
// clihehe/extras/string_view.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Extra things for std::string_view
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_EXTRAS_STRINGVIEW_H
#define NAO_CLIHEHE_EXTRAS_STRINGVIEW_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <utility>

namespace clihehe::nao {
struct string_literal : public std::string_view {
private:
  consteval string_literal(const char *A, size_t N) : std::string_view(A, N) {}

public:
  template <size_t N>
  consteval string_literal(const char (&A)[N]) : string_literal(A, N - 1) {}

  template <size_t N>
  [[nodiscard]] consteval static string_literal with_null(const char (&A)[N]) {
    return {A, N};
  }
};

template <size_t N> struct nttp_string {
  char S[N - 1];

  consteval nttp_string(const char (&S)[N]) { std::copy_n(S, N - 1, this->S); }

  consteval operator std::string_view() const { return {S, N - 1}; }
};

struct StringViewPP {
  static constexpr size_t npos = ~size_t(0);
  using iterator = const char *;
  using const_iterator = const char *;
  using size_type = size_t;
  using value_type = char;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  const char *S = nullptr;
  size_t N = 0;

  constexpr StringViewPP() = default;
  constexpr StringViewPP(const char *S, size_t N) : S(S), N(N) {}
  constexpr StringViewPP(std::string_view S) : S(S.data()), N(S.size()) {}

  [[nodiscard]] constexpr const char *data() const { return S; }
  [[nodiscard]] constexpr size_t size() const { return N; }

  constexpr operator std::string_view() const { return {S, N}; }

  constexpr size_t find(std::string_view Str, size_t From = 0) const {
    if (From > size())
      return npos;

    const char *Start = data() + From;
    size_t Size = size() - From;

    const char *Needle = Str.data();
    size_t N = Str.size();
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

    uint8_t BadCharSkip[256];
    std::memset(BadCharSkip, N, 256);
    for (unsigned i = 0; i != N - 1; ++i)
      BadCharSkip[(uint8_t)Str[i]] = N - 1 - i;

    do {
      uint8_t Last = Start[N - 1];
      if (Last == (uint8_t)Needle[N - 1]) [[unlikely]]
        if (std::memcmp(Start, Needle, N - 1) == 0)
          return Start - data();

      Start += BadCharSkip[Last];
    } while (Start < Stop);

    return npos;
  }

  [[nodiscard]] constexpr StringViewPP slice(size_t Start, size_t End) const {
    Start = std::min(Start, size());
    End = std::clamp(End, Start, size());
    return {data() + Start, End - Start};
  }

  [[nodiscard]] constexpr StringViewPP
  substr(size_t Start, size_t N = std::string_view::npos) const {
    Start = std::min(Start, size());
    return {data() + Start, std::min(N, size() - Start)};
  }

  constexpr std::pair<StringViewPP, StringViewPP> split(std::string_view Sep) {
    size_t Idx = find(Sep);
    if (Idx == npos)
      return {*this, {}};
    return {slice(0, Idx), substr(Idx + Sep.size())};
  }

  [[nodiscard]] constexpr iterator begin() const { return data(); }

  [[nodiscard]] constexpr iterator end() const { return data() + size(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() const {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] constexpr reverse_iterator rend() const {
    return std::make_reverse_iterator(begin());
  }
};
} // namespace clihehe::nao

#endif
