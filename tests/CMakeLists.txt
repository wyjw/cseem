enable_testing()

include_directories(
    ${CMAKE_SOURCE_DIR}
)

add_executable(
    parser_test
    parser_test.cpp
)

target_link_libraries(
    parser_test
    GTest::gtest_main
    cachelib
    parserlib
)

add_executable(
    set_tag_test
    set_tag_test.cpp
)

target_link_libraries(
    set_tag_test
    GTest::gtest_main
    cachelib
    parserlib
)

include(GoogleTest)

# add all the extra binaries to compare
add_custom_target("copycachescript")
add_custom_command(
    TARGET copycachescript POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/tests/from_cache_lib.py
            ${CMAKE_CURRENT_BINARY_DIR}/from_cache_lib.py
)

add_custom_target("copycachelist")
add_custom_command(
    TARGET copycachelist POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/tests/cache_lib.txt
            ${CMAKE_CURRENT_BINARY_DIR}/cache_lib.txt
)

add_custom_target("copytrace")
add_custom_command(
    TARGET copytrace POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/examples/hwparse.txt
            ${CMAKE_CURRENT_BINARY_DIR}/examples/hwparse.txt
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/examples/hwparse2.txt
            ${CMAKE_CURRENT_BINARY_DIR}/examples/hwparse2.txt
)

find_package(PythonInterp)
add_custom_target(
    run ALL
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/from_cache_lib.py
    # BYPRODUCTS  
    COMMENT "Generating comparison cache libs."
)

add_dependencies(parser_test copycachescript)
add_dependencies(parser_test copycachelist)
add_dependencies(parser_test copytrace)
add_dependencies(parser_test run)

gtest_discover_tests(parser_test)
gtest_discover_tests(set_tag_test)
