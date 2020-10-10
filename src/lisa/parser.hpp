#ifndef LISA_PARSER
#define LISA_PARSER
#include <lisa/lexer.hpp>
#include <string_theory/string>
#include <memory>
#include <vector>
#include <cstddef>

namespace lisa {
struct node {
  virtual ~node();
  virtual auto repr() const -> ST::string;
};

struct id : node {
  ST::string name;
  bool is_op;
  
  id(const ST::string &n, bool i) : name(n), is_op(i) {}
  
  auto repr() const -> ST::string;

  static auto parse(const std::vector<token> &, std::size_t &) -> std::unique_ptr<id>;
};

struct num : node {
  double number;
  
  num(double n) : number(n) {}

  auto repr() const -> ST::string;

  static auto parse(const std::vector<token> &, std::size_t &) -> std::unique_ptr<num>;
};

struct def : node {
  std::unique_ptr<id> fn_name;
  std::vector<std::unique_ptr<id>> args;
  std::vector<std::unique_ptr<node>> body;

  def(
      std::unique_ptr<id> &&f,
      std::vector<std::unique_ptr<id>> &&a,
      std::vector<std::unique_ptr<node>> &&b
  ) : fn_name(std::move(f)), args(std::move(a)), body(std::move(b)) {}

  auto repr() const -> ST::string;

  static auto parse(const std::vector<token> &, std::size_t &) -> std::unique_ptr<def>;
};

struct fn_call : node {
  std::unique_ptr<id> fn_name;
  std::vector<std::unique_ptr<node>> args;

  fn_call(
      std::unique_ptr<id> f,
      std::vector<std::unique_ptr<node>> a
  ) : fn_name(std::move(f)), args(std::move(a)) {}
  
  auto repr() const -> ST::string;

  static auto parse(const std::vector<token> &, std::size_t &) -> std::unique_ptr<fn_call>;
};

struct parser {
  auto parse(const std::vector<token> &) -> std::unique_ptr<node>;
  auto parse(const std::vector<token> &, std::size_t &i) -> std::unique_ptr<node>;
};
}

#endif
