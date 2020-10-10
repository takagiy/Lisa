#include <lisa/file.hpp>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

using ST::string;
using cppfs::FileHandle;
namespace fs { using namespace cppfs::fs; }

namespace lisa {
  auto read_file(const string &path) -> string {
    FileHandle file = fs::open(path.c_str());
    return file.readFile();
  }
}
