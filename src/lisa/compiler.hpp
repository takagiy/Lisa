#ifndef LISA_COMPILER
#define LISA_COMPILER

#include <lisa/parser.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <string_theory/string>
#include <unordered_map>
#include <vector>
#include <cstdlib>

namespace lisa {
struct variable {
  llvm::Value* value;
};

struct compiler {
  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module module;
  std::unordered_map<ST::string, variable> var_table;

  compiler() : context(), builder(context), module("mod", context), var_table() {}

  auto compile(const node &) -> void;
};

template <class Fn>
struct prim_fn {
  std::size_t argc;
  Fn generator;

  auto operator()(compiler &, const std::vector<node *> &) const -> llvm::Value*;
};
}

namespace lisa {
template<class Fn>
auto prim_fn<Fn>::operator()(compiler &c, const std::vector<node *> &v) const -> llvm::Value* {
  if (v.size() != this->argc) {
    return nullptr;
  }
  return generator(c, v);
}
}

#endif
