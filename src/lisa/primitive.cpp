#include <lisa/primitive.hpp>

namespace lisa {
prim_fn::prim_fn(const ST::string &name, const fn_type &t, raw_t* g) :
  t(t), generator(g) { prim_fn_map[name] = this; }

auto prim_fn::operator()(compiler &c, const std::vector<node *> &v) const -> llvm::Value* {
  return generator(c, v);
}

auto prim_fn::find(const ST::string &name) -> prim_fn* {
  if (auto it = prim_fn_map.find(name); it != prim_fn_map.end()) {
    return it->second;
  }
  return nullptr;
}
}
