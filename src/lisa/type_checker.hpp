#ifndef LISA_TYPE_CHECKER
#define LISA_TYPE_CHECKER

#include <string_theory/string>
#include <unordered_map>
#include <vector>

namespace lisa {
struct node;
struct type {
  ST::string name;

  type(const ST::string&);
  static auto of_str(const ST::string &) -> const type*;

  type(const type&) = delete;
  type(type&&) = delete;
  type& operator=(const type&) = delete;
  type& operator=(type&&) = delete;
};


inline std::unordered_map<ST::string, const type*> typename_map;
inline type f64("f64");
inline type statement("statement");

struct fn_type {
  type* ret;
  std::vector<type *> args;
};

struct type_checker {
  std::unordered_map<ST::string, fn_type> fn_table;
  std::unordered_map<ST::string, type*> var_table;
  type_checker() : fn_table(), var_table() {}

  auto type_check(const node &) -> void;
};
}

#endif
