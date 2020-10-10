#include <fmt/format.h>
#include <lisa/lexer.hpp>
#include <lisa/parser.hpp>
#include <lisa/file.hpp>

auto main(int argc, const char* argv[]) -> int {
  if (argc <= 1) {
    fmt::print("error: no input files\n");
    return -1;
  }
  auto code = lisa::read_file(argv[1]);
  auto lexer = lisa::lexer();
  auto tokens = lexer.tokenize(code);
  for(auto &&token: tokens) {
    fmt::print("{}: \"{}\"\n", str_of(token.kind).view(), token.raw.view());
  }
  auto parser = lisa::parser();
  auto ast = parser.parse(tokens);
  fmt::print("{}\n", ast->repr().view());
}
