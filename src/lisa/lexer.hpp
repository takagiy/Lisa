#ifndef LISA_LEXER
#define LISA_LEXER

#include <string_theory/string>
#include <vector>

namespace lisa {
enum class token_kind {
  lpar, rpar, word, num, op, invalid
};

auto str_of(token_kind) -> ST::string;

struct token {
  token_kind kind;
  ST::string raw;
};

struct lexer {
  auto tokenize(const ST::string &code) -> std::vector<token>;
};
}

#endif
