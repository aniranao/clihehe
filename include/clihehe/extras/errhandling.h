//============================================================================//
// clihehe/extras/errhandling.h
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Error report
///
//============================================================================//
// Copyright (c) 2026 AniraNao. All Rights Reserved.
//
// SPDX-License-Identifier: BSD-2-Clause
//============================================================================//

#ifndef NAO_CLIHEHE_EXTRAS_ERRHANDLING_H
#define NAO_CLIHEHE_EXTRAS_ERRHANDLING_H

#include "clihehe/fmt.h"
#include "clihehe/macro.h"
#include <cstdlib>
#include <string_view>

#ifdef NAO_CLIHH_EXCEPT
#  include <exception>
#endif

namespace clihehe::nao {
NAO_CLIHH_INLINE void fatal_error_report(std::string_view S) {
#if defined(NAO_CLIHH_EXCEPT)
  throw std::runtime_error("FATAL clihehe: " + S);
#endif

#ifdef NAO_CLIHH_PRT
  NAO_CLIHH_PRT::println(stderr, "FATAL clihehe: {}", S);
#else
  std::cerr << "FATAL clihehe: " << S << '\n';
#endif

#ifdef NAO_CLIHH_ABORT
  std::abort();
#elif defined(NAO_CLIHH_EXIT)
  std::exit(1);
#endif
}

} // namespace clihehe::nao

#endif
