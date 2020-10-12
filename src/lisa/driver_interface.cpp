#include <lisa/driver_interface.hpp>
#include <llvm/Support/raw_ostream.h>
#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>
#include <cstdlib>
#include <string>
#include <fmt/format.h>

using ST::string;
using llvm::raw_string_ostream;
namespace fs { using namespace cppfs::fs; }

namespace lisa {
auto make_executable(const string &out, const compiler &c) -> void {
  std::string ir;
  raw_string_ostream ss(ir);
  ss << c.module;
  ss.flush();

  fs::open("tmp.ll").writeFile(ir);
  std::system("llc --filetype obj tmp.ll");
  auto driver_cmd = fmt::format("gcc -o {} tmp.o", out.view());
  std::system(driver_cmd.c_str());
}
}
