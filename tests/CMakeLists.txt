enable_testing()

add_executable(
    parser_test
    parser_test.cpp
)

target_link_libraries(
    parser_test
    GTest::gtest_main
)

include(GoogleTest)

# add all the extra binaries to compare
add_custom_target("copy")
add_custom_command(
    TARGET copy PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/tests/from_cache_lib.py
            ${CMAKE_CURRENT_BINARY_DIR}/from_cache_lib.py
)

find_package(PythonInterp)
add_custom_target(
    run ALL
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/from_cache_lib.py
    # BYPRODUCTS  
    COMMENT "Generating comparison cache libs."
)
add_dependencies(parser_test copy)
add_dependencies(parser_test run)

gtest_discover_tests(parser_test)
