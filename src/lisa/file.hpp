#ifndef LISA_FILE
#define LISA_FILE
#include <string_theory/string>
#include <tl/expected.hpp>

namespace lisa {
  auto read_file(const ST::string &) -> tl::expected<ST::string, ST::string>;
}

#endif
