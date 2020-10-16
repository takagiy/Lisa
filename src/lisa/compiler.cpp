#include <lisa/type_checker.hpp>
#include <lisa/primitive.hpp>
#include <lisa/compiler.hpp>
#include <lisa/parser.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <algorithm>
#include <iterator>

using lisa::token_kind;
using lisa::compiler;
using llvm::FunctionType;
using llvm::ConstantInt;
using llvm::BasicBlock;
using llvm::ConstantFP;
using llvm::Function;
using llvm::APFloat;
using llvm::APInt;
using llvm::Value;
using llvm::Type;
using std::unordered_map;
using std::back_inserter;
using std::transform;
using std::vector;
using std::size_t;
using ST::string;

namespace lisa {
auto gen_fn_decl(compiler& c, const string& name, const fn_type& type) {
  if (auto p = prim_fn::find(name); p) {
    return;
  }
  vector<Type *> args_t;
  transform(type.args.cbegin(), type.args.cend(), back_inserter(args_t),
      [&](auto &&t) { return t->raw(c.context); });
  auto* ret_t = type.ret->raw(c.context);
  auto* fn_t = FunctionType::get(ret_t, args_t, false);
  auto* fn = Function::Create(
    fn_t,
    Function::ExternalLinkage,
    name.c_str(),
    c.module
  );
}

auto compiler::compile(const unordered_map<string, fn_type> &fn_table) -> void {
  for(auto&& [name, type]: fn_table) {
    gen_fn_decl(*this, name, type);
  }
}

auto compiler::compile(const node &ast) -> void {
  ast.gen(*this);
}

auto id::gen(compiler &c) const -> Value* {
  return c.var_table[this->name].value;
}

auto boolc::gen(compiler &c) const -> Value* {
  return c.builder.getInt1(this->value);
}

auto inum::gen(compiler &c) const -> Value* {
  return ConstantInt::get(c.context, APInt(32, this->number));
}

auto fnum::gen(compiler &c) const -> Value* {
  return ConstantFP::get(c.context, APFloat(this->number));
}

auto get_fn(compiler &c, const def & fn_def) -> Function* {
  if(auto* f = c.module.getFunction(fn_def.fn_name->name.c_str()); f) {
    return f;
  }
  else {
    return nullptr;
  }
}

auto def::gen(compiler &c) const -> Value* {
  Function* f = get_fn(c, *this);
  BasicBlock* block = BasicBlock::Create(c.context, "entry", f);
  c.builder.SetInsertPoint(block);

  c.var_table.clear();
  size_t i = 0;
  for(auto && a : f->args()) {
    c.var_table[this->args[i]->raw->name] = variable{&a};
    ++i;
  }

  if (body.empty()) {
    c.builder.CreateRetVoid();
  }
  else {
    for(size_t i = 0; i < this->body.size() - 1; ++i) {
      this->body[i]->gen(c);
    }
    auto* ret = this->body.back()->gen(c);
    c.builder.CreateRet(ret);
  }

  return f;
}

auto fn_call::gen(compiler &c) const -> Value* {
  if (auto prim = prim_fn::find(this->fn_name->name); prim) {
    return (*prim)(c, this->ref_args());
  }

  Function* f = c.module.getFunction(this->fn_name->name.c_str());

  vector<Value *> args;
  transform(this->args.begin(), this->args.end(), back_inserter(args),
      [&](auto &&a) { return a->gen(c); });

  return c.builder.CreateCall(f, args, "fncall");
}

auto progn::gen(compiler &c) const -> Value* {
  for(auto && ch : this->children) {
    ch->gen(c);
  }
  return nullptr;
}
}
