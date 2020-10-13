#include <lisa/file.hpp>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

using ST::string;
using cppfs::FileHandle;
namespace fs { using namespace cppfs::fs; }
using tl::expected;
using tl::make_unexpected;

namespace lisa {
  auto read_file(const string &path) -> expected<string,string> {
    FileHandle file = fs::open(path.c_str());
    if (file.isFile()) {
      return file.readFile();
    }
    else {
      return make_unexpected("File does not exists");
    }
  }
}
