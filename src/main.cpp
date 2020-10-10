#include <fmt/format.h>
#include <lisa/lexer.hpp>
#include <lisa/parser.hpp>
#include <lisa/type_checker.hpp>
#include <lisa/compiler.hpp>
#include <lisa/file.hpp>
#include <llvm/Support/raw_ostream.h>
#include <string>

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

  auto type_checker = lisa::type_checker();
  type_checker.type_check(*ast);

  for(auto &&[name, type]: type_checker.fn_table) {
    fmt::print("{}:\n", name.view());
    fmt::print("(");
    for(auto &&t: type.args) {
      fmt::print(" {}", t->name.view());
    }
    fmt::print(" ) -> {}\n", type.ret->name.view());
  }

  auto compiler = lisa::compiler();
  compiler.compile(*ast);

  std::string ir;
  llvm::raw_string_ostream ss(ir);

  ss << compiler.module;
  ss.flush();
  fmt::print("{}\n", ir);
}
