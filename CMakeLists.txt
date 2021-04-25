cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

project(SimpleOrderMatchingEngine)
set(CMAKE_CXX_FLAGS "-Wall -O3 -g -lrt")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

file(GLOB SOURCES src/*)
add_executable(testDemo ${SOURCES})
include_directories(include)

# google test
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/refs/tags/release-1.10.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# test
file(GLOB TEST_SOURCE test/*.cpp)
add_executable(runTests ${TEST_SOURCE})
include_directories(src)
target_link_libraries(runTests gtest gtest_main)
add_test(NAME run_test COMMAND runTests)

