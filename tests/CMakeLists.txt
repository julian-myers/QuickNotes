cmake_minimum_required(VERSION 3.20)

add_executable(RepositoryTests notes_repository_tests.cpp)
target_include_directories(RepositoryTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(RepositoryTests PRIVATE notes_lib GTest::gtest_main sqlite3)
include(GoogleTest)
gtest_discover_tests(RepositoryTests)
