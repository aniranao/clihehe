#ifndef NAO_CLIHEHE_FMT_H
#define NAO_CLIHEHE_FMT_H

#include "clihehe/macros.h"

#if defined(NAO_CLIHH_FMT_FORMAT) || defined(NAO_CLIHH_FMT_PRINT)
#  include "fmt/format.h"
#endif

#if defined(NAO_CLIHH_STD_FORMAT)
#  include <format>
#endif

#if defined(NAO_CLIHH_STD_PRINT)
#  include <print>
#endif

#ifdef NAO_CLIHH_FMT_FORMAT
#  define NAO_CLIHH_FMT_NS ::fmt
#elif defined(NAO_CLIHH_STD_FORMAT)
#  define NAO_CLIHH_FMT_NS ::std
#endif

#ifdef NAO_CLIHH_FMT_PRINT
#  define NAO_CLIHH_PRT_NS ::fmt
#elif defined(NAO_CLIHH_STD_PRINT)
#  define NAO_CLIHH_PRT_NS ::std
#endif

#endif
