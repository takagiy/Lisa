#include <lisa/type_checker.hpp>
#include <lisa/primitive.hpp>
#include <lisa/parser.hpp>
#include <string_theory/format>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

using type_t = lisa::type;
using std::back_inserter;
using std::transform;
using std::vector;
using std::pair;
using ST::string;
using ST::format;

namespace lisa {
type::type(const string &n, type::raw_t* r) : name(n), raw(r) {
  typename_map[n] = this;
}
auto type::of_str(const string &name) -> type* {
  return typename_map[name];
}

type_checker::type_checker() : fn_table(), var_table(), errors() {
  for(auto &&[name, p] : prim_fn_map) {
    fn_table[name] = p->t;
  }
}

auto type_checker::type_check(node &ast) -> void {
  ast.type(*this);
}

auto type_checker::expect(const token_pos &pos, type* expected, type* given) -> void {
  if (expected != given) {
    this->errors.push_back({
        pos, format("Expected type {}, but found {}", expected->name, given->name)});
  }
}

auto id::type(type_checker &t) -> type_t* {
  return t.var_table[this->name];
}

auto boolc::type(type_checker &t) -> type_t* {
  return &bool_;
}

auto inum::type(type_checker &t) -> type_t* {
  return &i32;
}

auto fnum::type(type_checker &t) -> type_t* {
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
    for (auto &&[op, name] : {pair{"+",  "__iadd"},
                                  {"-",  "__isub"},
                                  {"*",  "__imul"},
                                  {"/",  "__idiv"},
                                  {"=",  "__ieq"},
                                  {"+.", "__fadd"},
                                  {"-.", "__fsub"},
                                  {"*.", "__fmul"},
                                  {"/.", "__fdiv"},
                                  {"=.", "__feq"}}
    ) {
      if (this->fn_name->name == op) {
        this->fn_name->name = name;
        break;
      }
    }
  }

  size_t i = 0;
  for (auto &&a: this->args) {
    t.expect(a->pos, t.fn_table[this->fn_name->name].args[i], a->type(t));
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
