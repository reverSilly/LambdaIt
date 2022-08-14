cmake_minimum_required(VERSION 3.23)
project(LambdaIt)

set(CMAKE_CXX_STANDARD 23)

add_executable(LambdaIt main.cpp)

#add_compile_options(-Wall -Wextra -fconcepts-diagnostics-depth=999)
#target_compile_options(LambdaIt "-Wall -Wextra -fconcepts-diagnostics-depth=999")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fconcepts-diagnostics-depth=999")
