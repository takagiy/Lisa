#ifndef LISA_DRIVER_INTERFACE
#define LISA_DRIVER_INTERFACE
#include <lisa/compiler.hpp>
#include <string_theory/string>

namespace lisa {
auto make_executable(const ST::string&, const compiler &) -> void;
}

#endif

