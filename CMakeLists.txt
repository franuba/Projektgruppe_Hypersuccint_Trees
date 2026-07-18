cmake_minimum_required(VERSION 3.17)

# Some external submodules (e.g. extern/succinct_bv) still declare a
# cmake_minimum_required() version older than 3.5. Recent CMake releases
# (>= 4.0) removed compatibility with those old versions and refuse to
# configure such projects unless CMAKE_POLICY_VERSION_MINIMUM is set.
# Set it here so a plain "cmake -B build" works out of the box, without
# every contributor needing to remember to pass
# -DCMAKE_POLICY_VERSION_MINIMUM=3.5 by hand. Anyone who has already set
# it explicitly on the command line is left alone.
if(NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

project(ProjektSuccinctTrees VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

# The Cython/Python bindings under ./python rely on MSVC-specific compiler
# flags (e.g. "/std:c++17") and build a Windows .pyd extension module, so
# they are only supported when building with MSVC. Default this to ON on
# MSVC (preserving the previous behavior on Windows) and OFF everywhere
# else, so that plain GCC/Clang builds on Linux configure cleanly without
# needing to know about this flag. It can still be overridden explicitly,
# e.g. "-DBUILD_PYTHON=OFF" (as done in the CI workflow) or "-DBUILD_PYTHON=ON".
option(BUILD_PYTHON "Build the Cython/Python bindings (MSVC/Windows only)" ${MSVC})

add_subdirectory(src pht)
add_subdirectory(example_service)
add_subdirectory(tests)
if(NOT TARGET gtest)
    add_subdirectory(extern/googletest)
endif()
add_subdirectory(extern/irrxml)
add_subdirectory(extern/succinct_bv)
add_subdirectory(extern/thread-pool-2.0.0)
if(BUILD_PYTHON)
    if(NOT MSVC)
        message(WARNING "BUILD_PYTHON is ON but the compiler is not MSVC; the python bindings use MSVC-only compiler flags and will likely fail to build. Configure with -DBUILD_PYTHON=OFF to skip them.")
    endif()
    add_subdirectory(python)
else()
    message(STATUS "Skipping python bindings (BUILD_PYTHON=OFF)")
endif()
