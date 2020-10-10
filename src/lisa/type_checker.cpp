#include <lisa/type_checker.hpp>
#include <lisa/primitive.hpp>
#include <lisa/parser.hpp>
#include <vector>

using type_t = lisa::type;
using std::vector;

namespace lisa {
auto id::type(type_checker &t) const -> type_t* {
  return t.var_table[this->name];
}

auto num::type(type_checker &t) const -> type_t* {
  return &f64;
}

auto def::type(type_checker &t) const -> type_t* {
  auto arg_t = vector<type_t *>(this->args.size(), &f64);
  auto fn_t = fn_type {
    &f64,
    arg_t
  };
  t.fn_table[this->fn_name->name] = fn_t;
  return &statement;
}

auto fn_call::type(type_checker &t) const -> type_t* {
  if (this->fn_name->is_op) {
    if (this->fn_name->name == "+") {
      return prim_add.ret_type;
    }
    else if (this->fn_name->name == "-") {
      return prim_sub.ret_type;
    }
    else if (this->fn_name->name == "*") {
      return prim_mul.ret_type;
    }
  }
  else if (this->fn_name->name == "return") {
    return prim_return.ret_type;
  }
  return t.fn_table[this->fn_name->name].ret;
}

auto progn::type(type_checker &t) const -> type_t* {
  return &statement;
}
}
