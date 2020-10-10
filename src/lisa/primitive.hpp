#ifndef LISA_PRIMITIVE
#define LISA_PRIMITIVE
#include <lisa/compiler.hpp>
#include <lisa/parser.hpp>
#include <llvm/IR/Value.h>
#include <vector>

namespace lisa {
struct type;

template <class Fn>
struct prim_fn {
  type* ret_type;
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

inline prim_fn prim_add(&f64, 2, [](compiler &c, const std::vector<node *>& args) {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFAdd(lhs, rhs, "primadd");
});

inline prim_fn prim_sub(&f64, 2, [](compiler &c, const std::vector<node *>& args) {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFSub(lhs, rhs, "primsub");
});

inline prim_fn prim_mul(&f64, 2, [](compiler &c, const std::vector<node *>& args) {
  auto* lhs = args[0]->gen(c);
  auto* rhs = args[1]->gen(c);
  return c.builder.CreateFMul(lhs, rhs, "primmul");
});

inline prim_fn prim_return(&f64, 1, [](compiler &c, const std::vector<node *>& args) {
  auto* ret = args[0]->gen(c);
  return c.builder.CreateRet(ret);
});
}

#endif
