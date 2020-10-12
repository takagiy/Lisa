#ifndef LISA_LEXER
#define LISA_LEXER

#include <string_theory/string>
#include <cstdlib>
#include <vector>

namespace lisa {
enum class token_kind {
  lpar, rpar, word, inum, fnum, op, tysep, invalid
};

auto str_of(token_kind) -> ST::string;

struct token_pos {
  std::size_t line;
  std::size_t character;
};

struct token {
  token_pos pos;
  token_kind kind;
  ST::string raw;
};

struct lexer {
  auto tokenize(const ST::string &code) -> std::vector<token>;
};
}

#endif
