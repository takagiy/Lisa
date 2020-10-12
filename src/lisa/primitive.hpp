#ifndef LISA_PRIMITIVE
#define LISA_PRIMITIVE
#include <lisa/compiler.hpp>
#include <lisa/parser.hpp>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>

namespace lisa {
struct type;
extern type i32;
extern type f64;
extern type bool_;
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

inline prim_fn prim_and("and", &bool_, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateAnd(lhs, rhs, "primand");
});

inline prim_fn prim_or("or", &bool_, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateOr(lhs, rhs, "primor");
});

inline prim_fn prim_not("not", &bool_, 1, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* arg = args[0]->gen(c);
  return c.builder.CreateNot(arg, "primnot");
});

inline prim_fn prim_ieq("__ieq", &bool_, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateICmpEQ(lhs, rhs, "primeq");
});

inline prim_fn prim_feq("__feq", &bool_, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFCmpOEQ(lhs, rhs, "primeq");
});

inline prim_fn prim_iadd("__iadd", &i32, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateAdd(lhs, rhs, "primadd");
});

inline prim_fn prim_isub("__isub", &i32, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateSub(rhs, lhs, "primsub");
});

inline prim_fn prim_imul("__imul", &i32, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateMul(lhs, rhs, "primmul");
});

inline prim_fn prim_idiv("__idiv", &i32, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateSDiv(rhs, lhs, "primdiv");
});

inline prim_fn prim_fadd("__fadd", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFAdd(lhs, rhs, "primadd");
});

inline prim_fn prim_fsub("__fsub", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFSub(rhs, lhs, "primsub");
});

inline prim_fn prim_fmul("__fmul", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFMul(lhs, rhs, "primmul");
});

inline prim_fn prim_fdiv("__fdiv", &f64, 2, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFDiv(rhs, lhs, "primdiv");
});

inline prim_fn prim_return("return", &statement, 1, [](compiler &c, const std::vector<node *>& args) -> llvm::Value* {
  auto* ret = args[0]->gen(c);
  return c.builder.CreateRet(ret);
});
}

#endif
