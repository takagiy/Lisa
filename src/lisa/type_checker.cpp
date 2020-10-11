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

auto type_checker::type_check(const node &ast) -> void {
  ast.type(*this);
}

auto id::type(type_checker &t) const -> type_t* {
  return t.var_table[this->name];
}

auto num::type(type_checker &t) const -> type_t* {
  return &f64;
}

auto def::type(type_checker &t) const -> type_t* {
  t.var_table.clear();
  for (auto &&a : this->args) {
    t.var_table[a->raw->name] = &f64;
  }

  vector<type_t*> arg_t;
  transform(this->args.cbegin(), this->args.cend(), back_inserter(arg_t),
      [](auto &&a) { return type_t::of_str(a->ty_name->name); });
  auto ret_t = this->body.empty() ? &statement : this->body.back()->type(t);
  auto fn_t = fn_type {
    ret_t,
    arg_t
  };
  t.fn_table[this->fn_name->name] = fn_t;
  return &statement;
}

auto fn_call::type(type_checker &t) const -> type_t* {
  if (auto found = prim_fn::find(this->fn_name->name); found) {
    return found->ret_type;
  }
  return t.fn_table[this->fn_name->name].ret;
}

auto progn::type(type_checker &t) const -> type_t* {
  for (auto &&c : this->children) {
    c->type(t);
  }
  return &statement;
}
}
