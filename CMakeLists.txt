cmake_minimum_required(VERSION 3.20)
project(TuiNotes)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output binaries to build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
	tomlplusplus
	GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
	GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.15.1
)
FetchContent_MakeAvailable(spdlog)

find_package(Curses REQUIRED)
find_library(NCURSESW_LIBRARY NAMES ncursesw REQUIRED)

# ----------- Sources -------------------------------------------------------------

set(MARKDOWN_SOURCES
	src/markdown/Lexer.cpp
	src/markdown/Parser.cpp
	src/markdown/Renderer.cpp
	src/markdown/Colors.cpp
)

set(CONFIG_SOURCES
	src/config/ConfigLoader.cpp
	src/config/Editor.cpp
)

set(APP_SOURCES
	src/app/App.cpp
	src/app/state/MainMenuState.cpp
	src/app/state/NoteListState.cpp
	src/app/state/NewNoteState.cpp
	src/app/state/ViewingState.cpp
	src/app/state/DeleteNoteState.cpp
)

set(UI_SOURCES
	src/ui/MainMenu.cpp
	src/ui/NoteListWidget.cpp
	src/ui/PreviewWidget.cpp
	src/ui/AddNoteWidget.cpp
	src/ui/NoteViewWidget.cpp
	src/ui/DeleteConfirmWidget.cpp
)

set(DB_SOURCES
	src/db/Database.cpp
	src/db/NoteRepository.cpp
)

add_library(notes_lib STATIC
	${MARKDOWN_SOURCES}
	${CONFIG_SOURCES}
	${APP_SOURCES}
	${UI_SOURCES}
	${DB_SOURCES}
)

target_include_directories(notes_lib PUBLIC
	${CMAKE_SOURCE_DIR}/src
	${CURSES_INCLUDE_DIRS}
)

target_link_libraries(notes_lib PRIVATE
	${NCURSESW_LIBRARY}
	tomlplusplus::tomlplusplus
	sqlite3
	spdlog::spdlog
)

# ------------ Main Executable -----------------------------------------------------

add_executable(${PROJECT_NAME}
	src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE notes_lib)


# ---------------------- Tests ----------------------------------------------------
enable_testing()
add_executable(LexerTests tests/lexer_tests.cpp src/markdown/Lexer.cpp)
add_executable(ParserTests tests/parser_tests.cpp src/markdown/Parser.cpp src/markdown/Lexer.cpp)
target_include_directories(LexerTests PRIVATE src)
target_include_directories(ParserTests PRIVATE src)
target_link_libraries(LexerTests GTest::gtest_main)
target_link_libraries(ParserTests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(LexerTests)
gtest_discover_tests(ParserTests)
add_subdirectory(tests)

add_custom_target(check
	COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
	DEPENDS LexerTests ParserTests
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
