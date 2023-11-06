cmake_minimum_required(VERSION 3.12)
project(memory-test)
enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(
    googletest
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(test)
add_subdirectory(benchmark)
