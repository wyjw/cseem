cmake_minimum_required(VERSION 3.10)
project(cseem)
set(CMAKE_PROJECT_VERSION "1")
set(CMAKE_PROJECT_VERSION_MAJOR "1")
set(CMAKE_PROJECT_VERSION_MINOR "1")
set(CMAKE_PROJECT_VERSION_PATCH "1")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
