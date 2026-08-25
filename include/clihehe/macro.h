//============================================================================//
// clihehe/macro.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macro defination
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_MACRO_H
#define NAO_CLIHEHE_MACRO_H

// NOLINTNEXTLINE(modernize-macro-to-enum)
#define NAO_CLIHEHE_VERSION 100 // 0.1.0
#define NAO_CLIHEHE_VERSION_STRING "clihehe @ v0.1.0"

#include <version>

#if __cplusplus == 202002L
#  define NAO_CLIHH_CXX_20
#elif __cplusplus >= 202302L
#  define NAO_CLIHEHE_CXX_23
#else
#  error "clihehe: Wrong C++ version"
#endif

#ifdef __has_include
#  define NAO_CLIHH_HASINCLUDE(x) __has_include(x)
#else
#  define NAO_CLIHH_HASINCLUDE(x) 0
#endif

#ifdef __has_attribute
#  define NAO_CLIHH_HASATTRIBUTE(x) __has_attribute(x)
#else
#  define NAO_CLIHH_HASATTRIBUTE(x) 0
#endif

#ifdef __has_cpp_attribute
#  define NAO_CLIHH_CXXHASATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define NAO_CLIHH_CXXHASATTRIBUTE(x) 0
#endif

#if NAO_CLIHH_HASATTRIBUTE(always_inline)
#  define NAO_CLIHH_ALWAYS_INLINE_ATTR __attribute__((always_inline)) inline
#elif defined(CLIHEHE_MSVC)
#  define NAO_CLIHH_ALWAYS_INLINE_ATTR __forceinline
#else
#  define NAO_CLIHH_ALWAYS_INLINE_ATTR inline
#endif

#ifdef NDEBUG
#  define NAO_CLIHH_INLINE NAO_CLIHH_ALWAYS_INLINE_ATTR
#else
#  define NAO_CLIHH_INLINE inline
#endif

// Macro features:
//
// - NAO_CLIHEHE_FMT_FORMAT
// - NAO_CLIHEHE_FMT_PRINT
//
// - NAO_CLIHEHE_STD_PRINT
// - NAO_CLIHEHE_STD_FORMAT
//
// - NAO_CLIHEHE_STD_OSTREAM
//
// sstream doesn't supported

#if !defined(NAO_CLIHEHE_FMT_FORMAT) && !defined(NAO_CLIHEHE_STD_FORMAT) &&    \
    !defined(NAO_CLIHEHE_STD_SSTREAM)
#  if NAO_CLIHH_HASINCLUDE("fmt/format.h")
#    define NAO_CLIHEHE_FMT_FORMAT
#  elif defined(__cpp_lib_format)
#    define NAO_CLIHEHE_STD_FORMAT
#  endif
#endif

#if !defined(NAO_CLIHEHE_FMT_PRINT) && !defined(NAO_CLIHEHE_STD_PRINT) &&      \
    !defined(NAO_CLIHEHE_STD_OSTREAM)
#  if NAO_CLIHH_HASINCLUDE("fmt/format.h")
#    define NAO_CLIHEHE_FMT_PRINT
#  elif defined(__cpp_lib_print)
#    define NAO_CLIHEHE_STD_PRINT
#  else
#    define NAO_CLIHEHE_STD_OSTREAM
#  endif
#endif

#define NAO_CLIHH_VECTOR std::vector

#endif
