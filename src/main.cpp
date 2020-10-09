#include <fmt/format.h>
#include <lisa/lexer.hpp>
#include <lisa/parser.hpp>

auto main() -> int {
  auto code = "(def circle-area (r) (* (* r r) 3.14))";
  auto lexer = lisa::lexer();
  auto tokens = lexer.tokenize(code);
  for(auto &&token: tokens) {
    fmt::print("{}: \"{}\"\n", str_of(token.kind).view(), token.raw.view());
  }
  auto parser = lisa::parser();
  auto ast = parser.parse(tokens);
  fmt::print("{}\n", ast->repr().view());
}
