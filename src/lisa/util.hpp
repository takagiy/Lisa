#ifndef LISA_UTIL
#define LISA_UTIL

#include <lisa/lexer.hpp>
#include <string_theory/string>

namespace lisa {
struct error {
  token_pos pos;
  ST::string msg;
};
}

#endif
