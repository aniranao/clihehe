# clihehe - Another CLI parser for C++

WIP

## Usage

(This exists only in the author's mind; it hasn't become reality yet)

```cpp
// C++20
#include "clihehe/clihehe.h"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <iostream>

namespace ch = clihehe; // alias

inline bool support_color() {
  const char *CTERM = std::getenv("COLORTERM");

  return CTERM != nullptr && std::string_view{CTERM} == "truecolor";
}

int main(int argc, char **argv) {
  ch::CLIParser Pa("name_program", "desc");
  ch::positional<std::string> Str(Pa, ch::st::desc("desc"),
                                      ch::st::value_desc("STR"));
  ch::arg<bool> Color(Pa, 'C' /* short arg */, ch::st::long_opt("color") /* long arg */,
                      ch::st::desc("DESC"),
                      ch::st::default_val(support_color());
  ch::arg<uint32_t> weo(Pa, "weo" /* long */,
                              ch::st::desc("hehe"),
                              ch::st::default_val(5));

  auto IsSuccess = Pa.parse(argc, argv); // also: parse_exception
  if (!IsSuccess) {
    std::cerr << "Error: " << IsSuccess.message() << "\n";
    return 1;
  }

  std::cout << "Color: " << Color
            << "\nweo: " << weo
            << "\nStr: " << Str << "\n";

  return 0;
}
```

Help generated:

```sh
name_program - desc

Usage: name_program [options] <STR>

Positional Arguments:
  <STR>                      desc

Options:
  -C, --color                DESC (default: false)
      --weo <uint32_t>  hehe (default: 5)
  -h, --help                 Print help message
```

Example:

```sh
$ name_program hi --color --weo 32
Color: true
weo: 32
Str: hi
$ name_program 32 --weo=-1
Error: Parsing option '--weo': Invalid argument: '-1' # actually this message can change
$ name_program --weo=5 -- 4
Color: false
weo: 5
Str: 4
```

## Requires

Compiler with C++20 support, and STL at least supported `std::format` and `std::ranges`.
Also, `clihehe` supported with `fmtlib` or C++23.
Just depend on you!

# Acknowledge

API inspired by: [LLVM Support CommandLine](https://llvm.org/docs/CommandLine.html)
Source code: [llvm/Support/CommandLine.h](https://github.com/llvm/llvm-project/blob/main/llvm/include/llvm/Support/CommandLine.h)

LLVM licensed under: [Apache-2.0 WITH LLVM-exception](https://llvm.org/LICENSE.txt)

## License

BSD-2-Clause (temporary)
