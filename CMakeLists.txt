cmake_minimum_required(VERSION 3.18)
project(lisa VERSION 0.1.0 LANGUAGES CXX)

find_package(string_theory REQUIRED)
find_package(cppfs CONFIG REQUIRED)
find_package(LLVM CONFIG REQUIRED)
find_package(fmt REQUIRED)

add_library(ext_llvm INTERFACE)
target_link_libraries(ext_llvm INTERFACE LLVM)
target_compile_definitions(ext_llvm INTERFACE ${LLVM_DEFINITIONS})
target_include_directories(ext_llvm INTERFACE ${LLVM_INCLUDE_DIRS})

add_library(liblisa)
target_compile_features(liblisa PUBLIC cxx_std_20)
target_include_directories(liblisa PUBLIC src)
target_sources(liblisa PRIVATE
  src/lisa/lexer.cpp
  src/lisa/parser.cpp
  src/lisa/type_checker.cpp
  src/lisa/compiler.cpp
  src/lisa/primitive.cpp
  src/lisa/file.cpp)
target_link_libraries(liblisa PUBLIC
  string_theory
  cppfs::cppfs
  ext_llvm
  fmt)

add_executable(lisa src/main.cpp)
target_compile_features(lisa PUBLIC cxx_std_20)
target_link_libraries(lisa PUBLIC
  liblisa
  string_theory
  cppfs::cppfs
  fmt)
