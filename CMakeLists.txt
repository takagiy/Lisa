cmake_minimum_required(VERSION 3.18)
project(lisa VERSION 0.1.0 LANGUAGES CXX)

find_package(string_theory REQUIRED)
find_package(fmt REQUIRED)

add_library(liblisa)
target_compile_features(liblisa PUBLIC cxx_std_20)
target_include_directories(liblisa PUBLIC src)
target_sources(liblisa PUBLIC src/lisa/lexer.cpp)

add_executable(lisa src/main.cpp)
target_compile_features(lisa PUBLIC cxx_std_20)
target_link_libraries(lisa PUBLIC liblisa string_theory fmt)
