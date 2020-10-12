#include <lisa/lexer.hpp>
#include <cstdlib>
#include <cctype>

using std::vector;
using ST::string;
using lisa::token;
using lisa::token_kind;
using std::size_t;

namespace lisa {
auto str_of(token_kind kind) -> string {
  switch(kind) {
    case token_kind::lpar:
      return "lpar";
    case token_kind::rpar:
      return "rpar";
    case token_kind::word:
      return "word";
    case token_kind::inum:
      return "inum";
    case token_kind::fnum:
      return "fnum";
    case token_kind::tysep:
      return "tysep";
    case token_kind::op:
      return "op";
    default:
      return "invalid";
  }
}

auto str_of_char(char ch) {
  string result;
  return result += ch;
}

auto make_pos(size_t l, size_t c) {
  return token_pos {l, c + 1};
}

auto read_tokens_(vector<token> &result, const string& s, size_t line_n) {
  for(size_t i = 0; i < s.size(); ++i) {
    // consume whitespaces
    while(isspace(s[i])) {
      ++i;
    }

    // left paren
    if(s[i] == '(') {
      result.push_back(token{
          make_pos(line_n, i),
          token_kind::lpar,
          str_of_char(s[i])
      });
    }
    // right paren
    else if (s[i] == ')') {
      result.push_back(token{
          make_pos(line_n, i),
          token_kind::rpar,
          str_of_char(s[i])
      });
    }
    // identifier
    else if (isalpha(s[i])) {
      string read = str_of_char(s[i]);
      
      while(i + 1  < s.size() && (isalnum(s[i + 1]) || s[i + 1] == '-')) {
        ++i;
        read += s[i];
      }

      result.push_back(token{
          make_pos(line_n, i),
          token_kind::word,
          read
      });
    }
    // number
    else if (isalnum(s[i])) {
      string read = str_of_char(s[i]);

      while(i + 1 < s.size() && isdigit(s[i + 1])) {
        ++i;
        read += s[i];
      }

      // integer
      if (i + 1 >= s.size() || s[i + 1] != '.') {
        result.push_back(token{
            make_pos(line_n, i),
            token_kind::inum,
            read
        });
      }
      // floating point
      else {
        ++i;
        read += s[i];
        while(i + 1 < s.size() && isdigit(s[i + 1])) {
          ++i;
          read += s[i];
        }

        result.push_back(token{
            make_pos(line_n, i),
            token_kind::fnum,
            read
        });
      }
    }
    else if(s[i] == '\'') {
      result.push_back(token{
          make_pos(line_n, i),
          token_kind::tysep,
          str_of_char(s[i])
      });
    }
    // operator
    else if(ispunct(s[i])) {
      string read = str_of_char(s[i]);

      while(i + 1 < s.size() && ispunct(s[i + 1])) {
        ++i;
        read += s[i];
      }

      result.push_back(token{
          make_pos(line_n, i),
          token_kind::op,
          read
      });
    }
    // unknown
    else {
      result.push_back(token{
          make_pos(line_n, i),
          token_kind::invalid,
          str_of_char(s[i])
      });
    }
  }
}

auto lexer::tokenize(const string &code) -> vector<token> {
  vector<token> result{};

  size_t line_n = 1;
  for(auto &&s: code.split('\n')) {
    read_tokens_(result, s, line_n);
    ++line_n;
  }

  return result;
}
}
