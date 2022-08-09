cmake_minimum_required(VERSION 3.23)
project(LambdaIt)

set(CMAKE_CXX_STANDARD 23)

add_executable(LambdaIt main.cpp SyntaxTree.hpp)

add_compile_options("-ftemplate-depth=10")
