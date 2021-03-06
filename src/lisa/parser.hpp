#ifndef LISA_PARSER
#define LISA_PARSER
#include <lisa/lexer.hpp>
#include <lisa/util.hpp>
#include <llvm/IR/Value.h>
#include <string_theory/string>
#include <memory>
#include <vector>
#include <cstddef>

namespace lisa {
struct compiler;
struct type_checker;
struct type;
using type_t = type;

struct node;

struct parser {
  std::vector<error> errors;

  auto parse(const std::vector<token> &) -> std::unique_ptr<node>;
  auto parse(const std::vector<token> &, std::size_t &i) -> std::unique_ptr<node>;

  auto report(const token_pos&, const ST::string &);
  auto expect(const ST::string &, const token &) -> bool;
  auto expect(token_kind, const token &) -> bool;
};

struct node {
  token_pos pos;

  node(const token_pos &p) : pos(p) {}
  virtual ~node();
  virtual auto repr() const -> ST::string;
  virtual auto gen(compiler &) const -> llvm::Value* = 0;
  virtual auto type(type_checker &) -> type_t* = 0;
};

struct id : node {
  ST::string name;
  bool is_op;
  
  id(const token_pos &p, const ST::string &n, bool i) : node(p), name(n), is_op(i) {}
  
  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<id>;
};

struct boolc : node {
  bool value;

  boolc(const token_pos &p, bool v) : node(p), value(v) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser &, const std::vector<token> &, std::size_t &) -> std::unique_ptr<boolc>;
};

struct inum : node {
  unsigned long long number;
  
  inum(const token_pos &p, unsigned long long n) : node(p), number(n) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<inum>;
};

struct fnum : node {
  double number;
  
  fnum(const token_pos &p, double n) : node(p), number(n) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<fnum>;
};

template<class T>
struct typed : node {
  std::unique_ptr<id> ty_name;
  std::unique_ptr<T> raw;

  typed(
      const token_pos &p,
      std::unique_ptr<id> &&t,
      std::unique_ptr<T> &&r
  ) : node(p), ty_name(std::move(t)), raw(std::move(r)) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser&, std::unique_ptr<T>&&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<typed<T>>;
};

struct def : node {
  std::unique_ptr<id> fn_name;
  std::vector<std::unique_ptr<typed<id>>> args;
  std::vector<std::unique_ptr<node>> body;

  def(
      const token_pos &p,
      std::unique_ptr<id> &&f,
      std::vector<std::unique_ptr<typed<id>>> &&a,
      std::vector<std::unique_ptr<node>> &&b
  ) : node(p), fn_name(std::move(f)), args(std::move(a)), body(std::move(b)) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  static auto parse(parser&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<def>;
};

struct fn_call : node {
  std::unique_ptr<id> fn_name;
  std::vector<std::unique_ptr<node>> args;

  fn_call(
      const token_pos &p,
      std::unique_ptr<id> &&f,
      std::vector<std::unique_ptr<node>> &&a
  ) : node(p), fn_name(std::move(f)), args(std::move(a)) {}
  
  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;

  auto ref_args() const -> std::vector<node *>;

  static auto parse(parser&, const std::vector<token> &, std::size_t &) -> std::unique_ptr<fn_call>;
};

struct progn : node {
  std::vector<std::unique_ptr<node>> children;

  progn(
      const token_pos &p,
      std::vector<std::unique_ptr<node>> &&c
  ) : node(p), children(std::move(c)) {}

  auto repr() const -> ST::string;
  auto gen(compiler &) const -> llvm::Value*;
  auto type(type_checker &) -> type_t*;
};
}

#include <string_theory/format>
#include <vector>
#include <memory>

namespace lisa {
template<class T>
auto typed<T>::parse(parser& p, std::unique_ptr<T>&& raw, const std::vector<token> &t, size_t &i) -> std::unique_ptr<typed<T>> {
  if (p.expect(token_kind::tysep, t[i])) {
    return nullptr;
  }
  ++i;

  auto ty_name = id::parse(p, t, i);

  return std::make_unique<typed<T>>(t[i].pos, std::move(ty_name), std::move(raw));
}

template<class T>
auto typed<T>::repr() const -> ST::string {
  return ST::format("{{\"kind\":\"typed\", \"ty_name\":{}, \"raw\":{}}}", this->ty_name->repr(), this->raw->repr());
}

template<class T>
auto typed<T>::gen(compiler &) const -> llvm::Value* { return nullptr; }

template<class T>
auto typed<T>::type(type_checker &) -> type_t* { return nullptr; }
}

#endif
