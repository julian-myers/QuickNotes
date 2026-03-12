# Add test executables here, e.g.:
# file(GLOB_RECURSE TEST_SOURCES *.cpp)
cmake_minimum_required(VERSION 3.20)

add_executable(notes_tests
	lexer_tests.cpp
	parser_tests.cpp
)

target_include_directories(notes_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(notes_tests
	PRIVATE
	notes_lib
	GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(notes_tests)
# add_test(NAME tests COMMAND tests)
