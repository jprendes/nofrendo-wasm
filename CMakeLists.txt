cmake_minimum_required(VERSION 3.13)

project(nofrendo LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_subdirectory(src)
