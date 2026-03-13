#include "app/App.hpp"
#include "app/state/MainMenuState.hpp"
#include "config/ConfigLoader.hpp"
#include "db/Database.hpp"
#include "db/NoteRepository.hpp"
#include "markdown/Colors.hpp"
#include "utils/Log.hpp"
#include <clocale>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <ncurses.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace QuickNotes::App {

using namespace QuickNotes::App::State;

using Loader = QuickNotes::Config::ConfigLoader;

App::App()
    : m_config(std::make_shared<Config::Config>(Loader::load())), m_db(),
      m_repository(m_db) {
  setupLogger();
  setupCurses();
  auto initial =
      std::make_unique<MainMenuState>(stdscr, m_config, *this, m_repository);
  initial->onEnter();
  m_stateStack.push(std::move(initial));
};

App::~App() noexcept { endwin(); }

void App::run() {
  while (m_running) {
    m_stateStack.top()->render();
    int input = getch();
    if (auto next = m_stateStack.top()->handleInput(input)) {
      pushState(std::move(next));
    }
  }
}

void App::pushState(std::unique_ptr<AbstractState> state) {
  QN_LOG_DEBUG("pushState -> {}", state->name());
  state->onEnter();
  m_stateStack.push(std::move(state));
}

void App::popState() {
  QN_LOG_DEBUG("popState <- {}", m_stateStack.top()->name());
  m_stateStack.top()->onExit();
  m_stateStack.pop();
  if (!m_stateStack.empty()) {
    m_stateStack.top()->render();
  }
}

// ---- private methods ------

void App::setupLogger() {
  using path = std::filesystem::path;
  const char *xdg = std::getenv("XDG_DATA_HOME");
  path logDir =
      xdg ? path(xdg) : path(std::getenv("HOME")) / ".local" / "share";
  logDir /= "QuickNotes";
  std::filesystem::create_directories(logDir);
  auto logger =
      spdlog::basic_logger_mt("quicknotes", logDir / "quicknotes.log");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  QN_LOG_INFO("App initialized.");
}

void App::setupCurses() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  if (can_change_color()) {
    Markdown::Colors::initColorPairs(*m_config);
  }
  wbkgd(stdscr, COLOR_PAIR(Markdown::Colors::PAIR_TEXT));
}

} // namespace QuickNotes::App
