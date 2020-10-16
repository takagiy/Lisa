#include <lisa/parser.hpp>
#include <lisa/lexer.hpp>
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
auto parser::report(const token_pos &pos, const string &msg) {
  this->errors.emplace_back(pos, msg);
}

auto parser::expect(const string &word, const token &t) -> bool {
  if (word == t.raw) {
    return false;
  }
  else {
    this->report(t.pos, format("Expected \"{}\", but found \"{}\"",
          word, t.raw));
    return true;
  }
}

auto parser::expect(token_kind kind, const token &t) -> bool {
  if (kind == t.kind) {
    return false;
  }
  else {
    this->report(t.pos, format("Expected {}, but found \"{}\"",
          str_of(kind), t.raw));
    return true;
  }
}

auto forward(size_t &i, const vector<token> &t) {
  if (t[i].kind != token_kind::eof) {
    ++i;
  }
}

auto parser::parse(const vector<token> &t) -> uniq<node> {
  vector<uniq<node>> result;
  size_t i = 0;
  while(t[i].kind != token_kind::eof) {
    result.push_back(this->parse(t, i));
    forward(i, t);
  }
  return make_unique<progn>(token_pos{1, 1}, std::move(result));
}

auto parser::parse(const vector<token> &t, std::size_t &i) -> uniq<node> {
  if (t[i].kind == token_kind::lpar) {
    if (i + 1 >= t.size()) {
      return nullptr;
    }
    else if (t[i + 1].kind == token_kind::word && t[i + 1].raw == "def") {
      return def::parse(*this, t, i);
    }
    else if (t[i + 1].kind == token_kind::word || t[i + 1].kind == token_kind::op) {
      return fn_call::parse(*this, t, i);
    }
    else {
      this->report(t[i].pos, format("Unexpected token \"{}\"", t[i].raw));
      return nullptr;
    }
  }
  else if (t[i].kind == token_kind::word || t[i].kind == token_kind::op) {
    return id::parse(*this, t, i);
  }
  else if (t[i].kind == token_kind::inum) {
    return inum::parse(*this, t, i);
  }
  else if (t[i].kind == token_kind::fnum) {
    return fnum::parse(*this, t, i);
  }
  else {
    this->report(t[i].pos, format("Unexpected token \"{}\"", t[i].raw));
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

auto inum::repr() const -> string {
  return format("{{\"kind\":\"inum\", \"number\":{}}}", this->number);
}

auto fnum::repr() const -> string {
  return format("{{\"kind\":\"fnum\", \"number\":{}}}", this->number);
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


auto id::parse(parser&, const vector<token> &t, size_t &i) -> uniq<id> {
  return make_unique<id>(t[i].pos, t[i].raw, t[i].kind == token_kind::op);
}

auto inum::parse(parser&, const vector<token> &t, size_t &i) -> uniq<inum> {
  return make_unique<inum>(t[i].pos, t[i].raw.to_ulong_long());
}

auto fnum::parse(parser&, const vector<token> &t, size_t &i) -> uniq<fnum> {
  return make_unique<fnum>(t[i].pos, t[i].raw.to_double());
}

auto parse_body(parser& p, const vector<token> &t, size_t &i)  -> vector<uniq<node>> {
  vector<uniq<node>> result;

  while(t[i].kind != token_kind::eof && t[i].kind != token_kind::rpar) {
    result.push_back(p.parse(t, i));
    forward(i, t);
  }

  if (p.expect(token_kind::rpar, t[i])) {
    return {};
  }

  return result;
}

auto fn_call::parse(parser& p, const vector<token> &t, size_t &i) -> uniq<fn_call> {
  if (p.expect(token_kind::lpar, t[i])) {
    return nullptr;
  }
  forward(i, t);

  auto fn_name = id::parse(p, t, i);
  forward(i, t);

  auto args = parse_body(p, t, i);

  return make_unique<fn_call>(t[i].pos, std::move(fn_name), std::move(args));
}

auto parse_def_args(parser& p, const vector<token> &t, size_t &i) -> vector<uniq<typed<id>>> {
  if (p.expect(token_kind::lpar, t[i])) {
    return {};
  }
  forward(i, t);

  vector<uniq<typed<id>>> result;

  while(t[i].kind != token_kind::eof && t[i].kind == token_kind::word) {
    auto arg_name = id::parse(p, t, i);
    forward(i, t);
    result.push_back(typed<id>::parse(p, std::move(arg_name), t, i));
    forward(i, t);
  }

  if (p.expect(token_kind::rpar, t[i])) {
    return {};
  }

  return result;
}

auto def::parse(parser& p, const vector<token> &t, size_t &i) -> uniq<def> {
  if (p.expect(token_kind::lpar, t[i])) {
    return nullptr;
  }
  forward(i, t);

  if (p.expect("def", t[i])) {
    return nullptr;
  }
  forward(i, t);

  auto fn_name = id::parse(p, t, i);
  forward(i, t);

  auto args = parse_def_args(p, t, i);
  forward(i, t);

  auto body = parse_body(p, t, i);

  return make_unique<def>(t[i].pos, std::move(fn_name), std::move(args), std::move(body));
}
}
