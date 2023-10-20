cmake_minimum_required(VERSION 3.12)

project(memory-test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O0 -fno-inline -fprofile-arcs -ftest-coverage")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
file(GLOB_RECURSE TESTS src/*.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${TESTS})
add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE inc)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE gtest)
include(GoogleTest)
gtest_discover_tests(${CMAKE_PROJECT_NAME})
