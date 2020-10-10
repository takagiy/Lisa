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
#include <algorithm>
#include <iterator>

using lisa::token_kind;
using lisa::compiler;
using llvm::FunctionType;
using llvm::BasicBlock;
using llvm::ConstantFP;
using llvm::Function;
using llvm::APFloat;
using llvm::Value;
using llvm::Type;
using std::back_inserter;
using std::transform;
using std::vector;
using std::size_t;

namespace lisa {
auto compiler::compile(const node &ast) -> void {
  ast.gen(*this);
}

auto id::gen(compiler &c) const -> Value* {
  return c.var_table[this->name].value;
}

auto num::gen(compiler &c) const -> Value* {
  return ConstantFP::get(c.context, APFloat(this->number));
}

auto get_fn(compiler &c, const def & fn_def) {
  if(Function* f = c.module.getFunction(fn_def.fn_name->name.c_str()); f) {
    return f;
  }

  vector<Type *> arg_types(fn_def.args.size(), Type::getDoubleTy(c.context));
  FunctionType* fn_type = FunctionType::get(Type::getDoubleTy(c.context), arg_types, false);

  Function* f = Function::Create(
      fn_type,
      Function::ExternalLinkage,
      fn_def.fn_name->name.c_str(),
      c.module);

  size_t i = 0;
  for(auto && a : f->args()) {
    a.setName(fn_def.args[i]->name.c_str());
    ++i;
  }

  return f;
}

auto def::gen(compiler &c) const -> Value* {
  Function* f = get_fn(c, *this);
  BasicBlock* block = BasicBlock::Create(c.context, "entry", f);
  c.builder.SetInsertPoint(block);

  c.var_table.clear();
  for(auto && a : f->args()) {
    c.var_table[a.getName().data()] = variable{&a};
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
  if (this->fn_name->is_op) {
    if (this->fn_name->name == "+") {
      return prim_add(c, this->ref_args());
    }
    else if (this->fn_name->name == "-") {
      return prim_sub(c, this->ref_args());
    }
    else if (this->fn_name->name == "*") {
      return prim_mul(c, this->ref_args());
    }
  }
  else if (this->fn_name->name == "return") {
    return prim_return(c, this->ref_args());
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
