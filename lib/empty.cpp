#include "clihehe/clihehe.h"
#include "clihehe/option_parser.h"
#include "clihehe/parser_context.h"
#include "clihehe/types_parser.h"

auto a() {
  std::vector<int> A;

  return clihehe::parser{}.parse(A, "hehe, hehe");
}
