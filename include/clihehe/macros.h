//============================================================================//
// clihehe/macros.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macro defintions
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_MACROS_H
#define NAO_CLIHEHE_MACROS_H

#include <version>

#if __cplusplus == 202002L
#  define NAO_CLIHH_CXX_20
#elif __cplusplus >= 202302L
#  define NAO_CLIHH_CXX_23
#else
#  error "nao/clihehe: wrong C++ version"
#endif

#ifndef __has_include
#  define NAO_CLIHH_HAS_INCLUDE(x) 0
#else
#  define NAO_CLIHH_HAS_INCLUDE(x) __has_include(x)
#endif

#ifndef __has_attribute
#  define NAO_CLIHH_HAS_ATTRIBUTE(x) 0
#else
#  define NAO_CLIHH_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif

#ifndef __has_cpp_attribute
#  define NAO_CLIHH_HAS_CPP_ATTRIBUTE(x) 0
#else
#  define NAO_CLIHH_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif

#ifdef __GNUC__
#  define NAO_CLIHH_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#  define NAO_CLIHH_ALWAYS_INLINE __forceinline
#else
#  define NAO_CLIHH_ALWAYS_INLINE inline
#endif

#ifndef NDEBUG
#  define NAO_CLIHH_INLINE inline
#else
#  define NAO_CLIHH_INLINE NAO_CLIHH_ALWAYS_INLINE
#endif

//===----------------------------------------------------===//
// Options:
// - NAO_CLIHH_FMT_FORMAT - fmt::format
// - NAO_CLIHH_STD_FORMAT - std::format
//
// - NAO_LICHH_FMT_PRINT - fmt::print
// - NAO_CLIHH_STD_PRINT - std::print
// - NAO_CLIHH_STD_OSTREAM - std::ostream
//===----------------------------------------------------===//

#if !defined(NAO_CLIHH_FMT_FORMAT) && !defined(NAO_CLIHH_STD_FORMAT)
#  if NAO_CLIHH_HAS_INCLUDE("fmt/base.h")
#    define NAO_CLIHH_FMT_FORMAT
#  else
#    define NAO_CLIHH_STD_FORMAT
#  endif
#endif

#if !defined(NAO_CLIHH_FMT_PRINT) && !defined(NAO_CLIHH_STD_PRINT) &&          \
    !defined(NAO_CLIHH_STD_OSTREAM)
#  if NAO_CLIHH_HAS_INCLUDE("fmt/base.h")
#    define NAO_CLIHH_FMT_PRINT
#  elif defined(NAO_CLIHH_CXX_23) && defined(__cpp_lib_print)
#    define NAO_CLIHH_STD_PRINT
#  else
#    define NAO_CLIHH_OSTREAM
#  endif
#endif

#endif
