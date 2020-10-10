#include <lisa/parser.hpp>
#include <string_theory/format>
#include <algorithm>
#include <iterator>

using std::transform;
using std::back_inserter;
using std::vector;
template<class T>
using uniq = std::unique_ptr<T>;
using std::make_unique;
using ST::string;
using ST::format;
using std::size_t;
using lisa::token;
using lisa::token_kind;

namespace lisa {
auto parser::parse(const vector<token> &t) -> uniq<node> {
  vector<uniq<node>> result;
  size_t i = 0;
  while(i < t.size()) {
    result.push_back(this->parse(t, i));
    ++i;
  }
  return make_unique<progn>(std::move(result));
}

auto parser::parse(const vector<token> &t, std::size_t &i) -> uniq<node> {
  if (t[i].kind == token_kind::lpar) {
    if (i + 1 >= t.size()) {
      return nullptr;
    }
    else if (t[i + 1].kind == token_kind::word && t[i + 1].raw == "def") {
      return def::parse(t, i);
    }
    else if (t[i + 1].kind == token_kind::word || t[i + 1].kind == token_kind::op) {
      return fn_call::parse(t, i);
    }
    else {
      return nullptr;
    }
  }
  else if (t[i].kind == token_kind::word || t[i].kind == token_kind::op) {
    return id::parse(t, i);
  }
  else if (t[i].kind == token_kind::num) {
    return num::parse(t, i);
  }
  else {
    return nullptr;
  }
}

node::~node() {}

auto node::repr() const -> string {
  return "<node>";
}

auto id::repr() const -> string {
  return format("{{\"kind\":\"id\", \"name\":\"{}\", \"is_op\": {}}}",
      this->name,
      this->is_op);
}

auto num::repr() const -> string {
  return format("{{\"kind\":\"num\", \"number\":{}}}", this->number);
}

template <class T>
auto repr_body(const vector<uniq<T>> &body) -> string {
  if (body.empty()) {
    return "[]";
  }

  string result = "[" + body.front()->repr();
  for(size_t i = 1; i < body.size(); ++i) {
    result += ", ";
    result += body[i]->repr();
  }
  result += "]";
  return result;
}

auto def::repr() const -> string {
  return format("{{\"kind\":\"def\", \"fn_name\":{}, \"args\":{}, \"body\":{}}}",
      this->fn_name->repr(),
      repr_body(this->args),
      repr_body(this->body));
}

auto fn_call::repr() const -> string {
  return format("{{\"kind\":\"fn_call\", \"fn_name\":{}, \"args\":{}}}",
      this->fn_name->repr(),
      repr_body(this->args));
}

auto progn::repr() const -> string {
  return repr_body(this->children);
}

auto fn_call::ref_args() const -> vector<node *> {
  vector<node *> result;
  transform(this->args.cbegin(), this->args.cend(), back_inserter(result),
      [](auto &&p) { return p.get(); });
  return result;
}


auto id::parse(const vector<token> &t, size_t &i) -> uniq<id> {
  return make_unique<id>(t[i].raw, t[i].kind == token_kind::op);
}

auto num::parse(const vector<token> &t, size_t &i) -> uniq<num> {
  return make_unique<num>(t[i].raw.to_double());
}

auto parse_body(const vector<token> &t, size_t &i)  -> vector<uniq<node>> {
  vector<uniq<node>> result;
  parser parser;
  auto parsed = parser.parse(t, i);

  while(parsed) {
    result.push_back(std::move(parsed));
    ++i;
    parsed = parser.parse(t, i);
  }

  if (t[i].kind != token_kind::rpar) {
    return {};
  }

  return result;
}

auto fn_call::parse(const vector<token> &t, size_t &i) -> uniq<fn_call> {
  if (t[i].kind != token_kind::lpar) {
    return nullptr;
  }
  ++i;

  auto fn_name = id::parse(t, i);
  ++i;

  auto args = parse_body(t, i);

  return make_unique<fn_call>(std::move(fn_name), std::move(args));
}

auto parse_def_args(const vector<token> &t, size_t &i) -> vector<uniq<typed<id>>> {
  if (t[i].kind != token_kind::lpar) {
    return {};
  }
  ++i;
  
  vector<uniq<typed<id>>> result;

  while(i < t.size() && t[i].kind == token_kind::word) {
    auto arg_name = id::parse(t, i);
    ++i;
    result.push_back(typed<id>::parse(std::move(arg_name), t, i));
    ++i;
  }
  
  if (t[i].kind != token_kind::rpar) {
    return {};
  }

  return result;
}

auto def::parse(const vector<token> &t, size_t &i) -> uniq<def> {
  if (t[i].kind != token_kind::lpar) {
    return nullptr;
  }
  ++i;

  if (t[i].raw != "def") {
    return nullptr;
  }
  ++i;

  auto fn_name = id::parse(t, i);
  ++i;

  auto args = parse_def_args(t, i);
  ++i;

  auto body = parse_body(t, i);

  return make_unique<def>(std::move(fn_name), std::move(args), std::move(body));
}
}
