cmake_minimum_required(VERSION 3.10)
project(cseem)
set(CMAKE_PROJECT_VERSION "1")
set(CMAKE_PROJECT_VERSION_MAJOR "1")
set(CMAKE_PROJECT_VERSION_MINOR "1")
set(CMAKE_PROJECT_VERSION_PATCH "1")

# set C++20 properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(cachelib
    Cache.hpp
    Cache.cpp
)

add_library(parserlib
    Parser.hpp
    Parser.cpp
)

add_executable(main
    Cache.hpp
    Parser.hpp
    Main.cpp
    Cache.cpp
    Parser.cpp
)

enable_testing()
add_subdirectory(tests)
