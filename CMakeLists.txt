cmake_minimum_required(VERSION 3.17)
project(ProjektSuccinctTrees VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_subdirectory(src pht)
add_subdirectory(example_service)
add_subdirectory(tests)
if(NOT TARGET gtest)
    add_subdirectory(extern/googletest)
endif()
add_subdirectory(extern/irrxml)
add_subdirectory(extern/succinct_bv)
add_subdirectory(extern/thread-pool-2.0.0)
#add_subdirectory(python)

add_compile_options(-fms-extensions)

