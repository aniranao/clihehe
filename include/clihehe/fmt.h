//============================================================================//
// clihehe/fmt.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Automatically include formatting header
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_FMT_H
#define NAO_CLIHEHE_FMT_H

#include "clihehe/macro.h"

#if defined(NAO_CLIHEHE_FMT_FORMAT) || defined(NAO_CLIHEHE_FMT_PRINT)
#  include "fmt/format.h"
#endif

#if defined(NAO_CLIHEHE_FMT_FORMAT)
#  define NAO_CLIHH_FMT ::fmt
#elif defined(NAO_CLIHEHE_STD_FORMAT)
#  include <format>
#  define NAO_CLIHH_FMT ::std
#endif

#if defined(NAO_CLIHEHE_FMT_PRINT)
#  define NAO_CLIHH_PRT ::fmt
#elif defined(NAO_CLIHEHE_STD_PRINT)
#  include <print>
#  define NAO_CLIHH_PRT ::std
#elif defined(NAO_CLIHEHE_STD_OSTREAM)
#  include <iostream>
#endif

#ifndef NAO_CLIHH_FMT
#  include <format>
#  define NAO_CLIHH_FMT ::std
#endif

#endif
