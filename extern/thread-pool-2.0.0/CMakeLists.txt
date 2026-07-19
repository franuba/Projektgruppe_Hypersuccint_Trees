#cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
#project(thread-pool-2.0.0)

#set (CMAKE_CXX_STANDARD 17)

file(GLOB thread-pool_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.h" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.hpp" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.c" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.cpp" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.cc" "*.h" "*.hpp" "*.c" "*.cpp" "*.cc")
# thread_pool_test.cpp is a standalone benchmark/test program with its own
# main(). It must not be compiled into the thread-pool-2.0.0 library itself:
# doing so both bakes a stray main() into a library and fails to build on
# GCC/Clang, since that file relies on a few standard headers
# (<algorithm>, <numeric>, etc.) that happen to be pulled in transitively by
# MSVC's headers but are not included explicitly.
list(FILTER thread-pool_src EXCLUDE REGEX "thread_pool_test\\.cpp$")
# thread_pool.hpp is a header-only library (per its own documentation: just
# drop the header in and #include it), so once the test executable above is
# excluded there is no compilable .cpp/.c/.cc left in this directory at all.
# An INTERFACE library reflects that correctly: it only propagates include
# directories and compile features to whatever links against it, without
# CMake needing to determine a link language or compile/link anything here.
add_library(thread-pool-2.0.0 INTERFACE)
target_sources(thread-pool-2.0.0 INTERFACE ${thread-pool_src})
target_include_directories(thread-pool-2.0.0 INTERFACE "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0")
target_compile_features(thread-pool-2.0.0 INTERFACE cxx_std_17)
