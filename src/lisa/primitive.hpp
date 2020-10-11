#ifndef LISA_PRIMITIVE
#define LISA_PRIMITIVE
#include <lisa/compiler.hpp>
#include <lisa/parser.hpp>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>

namespace lisa {
struct type;
extern type f64;
extern type statement;

struct prim_fn {
  using raw_t = llvm::Value* (compiler&, const std::vector<node *>&);
  type* ret_type;
  std::size_t argc;
  raw_t* generator;

  prim_fn(const ST::string &, type*, std::size_t, raw_t*);
  auto operator()(compiler &, const std::vector<node *> &) const -> llvm::Value*;

  static auto find(const ST::string &) -> prim_fn*;
};

inline std::unordered_map<ST::string, prim_fn*> prim_fn_map;

inline prim_fn prim_fadd("__fadd", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFAdd(lhs, rhs, "primadd");
});

inline prim_fn prim_fsub("__fsub", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFSub(lhs, rhs, "primsub");
});

inline prim_fn prim_fmul("__fmul", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFMul(lhs, rhs, "primmul");
});

inline prim_fn prim_return("return", &statement, 1, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* ret = args[0]->gen(c);
  return c.builder.CreateRet(ret);
});
}

#endif
