#include <lisa/type_checker.hpp>
#include <lisa/primitive.hpp>
#include <lisa/parser.hpp>
#include <algorithm>
#include <iterator>
#include <vector>

using type_t = lisa::type;
using std::back_inserter;
using std::transform;
using std::vector;
using ST::string;

namespace lisa {
type::type(const string &n, type::raw_fn_t* r) : name(n), raw(r) {
  typename_map[n] = this;
}
auto type::of_str(const string &name) -> type* {
  return typename_map[name];
}

auto type_checker::type_check(node &ast) -> void {
  ast.type(*this);
}

auto id::type(type_checker &t) -> type_t* {
  return t.var_table[this->name];
}

auto num::type(type_checker &t) -> type_t* {
  return &f64;
}

auto def::type(type_checker &t) -> type_t* {
  vector<type_t*> arg_t;
  t.var_table.clear();

  for (auto &&a : this->args) {
    auto* at = type_t::of_str(a->ty_name->name);
    t.var_table[a->raw->name] = at;
    arg_t.push_back(at);
  }
  for (auto &&b : this->body) {
    b->type(t);
  }

  auto ret_t = this->body.empty() ? &statement : this->body.back()->type(t);
  auto fn_t = fn_type {
    ret_t,
    arg_t
  };
  t.fn_table[this->fn_name->name] = fn_t;
  return &statement;
}

auto fn_call::type(type_checker &t) -> type_t* {
  for (auto &&a : this->args) {
    a->type(t);
  }
  if (this->fn_name->is_op) {
    if (this->fn_name->name == "+") {
      this->fn_name->name = "__fadd";
    }
    else if (this->fn_name->name == "-") {
      this->fn_name->name = "__fsub";
    }
    else if (this->fn_name->name == "*") {
      this->fn_name->name = "__fmul";
    }
  }
  if (auto found = prim_fn::find(this->fn_name->name); found) {
    return found->ret_type;
  }
  return t.fn_table[this->fn_name->name].ret;
}

auto progn::type(type_checker &t) -> type_t* {
  for (auto &&c : this->children) {
    c->type(t);
  }
  return &statement;
}
}
