#ifndef LISA_TYPE_CHECKER
#define LISA_TYPE_CHECKER

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <string_theory/string>
#include <unordered_map>
#include <vector>

namespace lisa {
struct node;
struct type {
  using raw_t = llvm::Type* (llvm::LLVMContext &);

  ST::string name;
  raw_t* raw;

  type(const ST::string&, raw_t*);
  static auto of_str(const ST::string &) -> type*;

  type(const type&) = delete;
  type(type&&) = delete;
  type& operator=(const type&) = delete;
  type& operator=(type&&) = delete;
};


inline std::unordered_map<ST::string, type*> typename_map;
inline type i32("i32", (type::raw_t*)(&llvm::Type::getInt32Ty));
inline type f64("f64", &llvm::Type::getDoubleTy);
inline type bool_("bool", (type::raw_t*)(&llvm::Type::getInt1Ty));
inline type statement("statement", &llvm::Type::getVoidTy);

struct fn_type {
  type* ret;
  std::vector<type *> args;
};

struct type_checker {
  std::unordered_map<ST::string, fn_type> fn_table;
  std::unordered_map<ST::string, type*> var_table;
  type_checker() : fn_table(), var_table() {}

  auto type_check(node &) -> void;
};
}

#endif
